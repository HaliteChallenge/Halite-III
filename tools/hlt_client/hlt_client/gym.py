import datetime
import json
import os
import random
import sqlite3
import sys
import subprocess

import appdirs
import trueskill

from . import compare_bots, output, util


APP_NAME = 'hlt_client3'
APP_AUTHOR = 'Halite'

BOTS_MODE = 'bots'
EVALUATE_MODE = 'evaluate'
REGISTER_MODE = 'register'
DEREGISTER_MODE = 'deregister'
STATS_MODE = 'stats'

BASE_MU = 25.0
BASE_SIGMA = 8.333
MIN_PLAYERS = 2

SCHEMA = '''
create table hlt_client_version (version INTEGER);
create table bots (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE,
    version INTEGER,
    mu REAL,
    sigma REAL,
    path TEXT,
    games_played INTEGER,
    rank INTEGER
);
create table games (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    datetime TEXT,
    winner INTEGER,
    participants JSON,
    results JSON,
    FOREIGN KEY(winner) REFERENCES bots(id)
);
create table rank_history (
    bot_id INTEGER,
    rank INTEGER,
    datetime TEXT,
    mu REAL,
    sigma REAL,
    FOREIGN KEY(bot_id) REFERENCES bots(id)
)
'''

def connect(db_path=None):
    if not db_path:
        db_path = os.path.join(appdirs.user_data_dir(APP_NAME, APP_AUTHOR), 'gym.db')

    os.makedirs(os.path.dirname(db_path), exist_ok=True)

    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row

    # Make sure database is initialized
    try:
        conn.execute('select * from hlt_client_version')
    except sqlite3.OperationalError:
        initialize_db(conn)

    return conn


def initialize_db(conn):
    conn.executescript(SCHEMA)


def rerank_bots(conn):
    all_bots = conn.execute('select * from bots').fetchall()
    all_bots.sort(reverse=True, key=lambda bot: bot['mu'] - 3 * bot['sigma'])
    for index, bot in enumerate(all_bots):
        rank = index + 1
        conn.execute('update bots set rank = ? where id = ?', (rank, bot['id']))


def register_bot(conn, name, path):
    # If bot with name exists, update bot, else insert record
    existing = conn.execute('select * from bots where name = ?', (name,)).fetchall()
    if existing:
        query = 'update bots set games_played = 0, version = version + 1, sigma = ?, path = ? where id = ?'
        conn.execute(query, (BASE_SIGMA, path, existing[0]['id']))
    else:
        query = 'insert into bots (name, version, mu, sigma, path, games_played) values (?, ?, ?, ?, ?, 0)'
        conn.execute(query, (name, 1, BASE_MU, BASE_SIGMA, path))

    rerank_bots(conn)


def deregister_bot(conn, name):
    existing = conn.execute('select * from bots where name = ?', (name,)).fetchall()
    if existing and util.confirm("Delete bot {}?".format(existing[0]['id']), json_confirm=True):
        query = 'delete from bots where id = ?'
        conn.execute(query, (existing[0]['id'],))
        output.output("Bot deregistered.")
    else:
        output.output("No bot to deregister.")

    rerank_bots(conn)


def list_bots(conn):
    bots = conn.execute('select * from bots').fetchall()
    return [dict(bot) for bot in bots]


def add_match(conn, bots, results):
    winner = None
    for id_str, stats in results['stats'].items():
        if stats['rank'] == 1:
            winner = int(id_str)

    if winner is None:
        raise ValueError('Could not detect winner of game')

    query = 'insert into games (datetime, winner, participants, results) values (?, ?, ?, ?)'
    del results['final_snapshot']
    current_time = datetime.datetime.now().isoformat()
    conn.execute(query, (current_time,
                         bots[winner]['id'],
                         json.dumps(bots),
                         json.dumps(results)))

    for bot in bots:
        history_query = 'insert into rank_history (bot_id, datetime, rank, mu, sigma) values (?, ?, ?, ?, ?)'
        conn.execute(history_query, (bot['id'],
                                     current_time,
                                     bot['rank'],
                                     bot['mu'],
                                     bot['sigma']))
        games_played_query = 'update bots set games_played=games_played + 1 where id = ?'
        conn.execute(games_played_query, (bot['id'],))

    trueskill.setup(tau=0.008, draw_probability=0.001)
    teams = [[trueskill.Rating(mu=bot["mu"], sigma=bot["sigma"])]
             for bot in bots]
    ranks = [results["stats"][str(b)]["rank"] - 1 for b in range(len(bots))]
    new_ratings = trueskill.rate(teams, ranks)

    update_query = 'update bots set mu=?, sigma=? where id=?'
    for bot, rating in zip(bots, new_ratings):
        conn.execute(update_query, (rating[0].mu, rating[0].sigma, bot['id']))

    rerank_bots(conn)


def run_matches(db_path, hlt_path, output_dir, iterations):
    flags = []

    if output_dir:
        abs_output_dir = os.path.abspath(output_dir)
        os.makedirs(abs_output_dir, exist_ok=True)
        flags = ['-i', abs_output_dir]

    for i in range(iterations):
        with connect(db_path) as conn:
            all_bots = list_bots(conn)
            num_players = random.choice((2, 4))
            if len(all_bots) < MIN_PLAYERS:
                output.error('Need at least {} bots registered to play.'.format(MIN_PLAYERS))
                sys.exit(1)
            elif len(all_bots) < num_players:
                num_players = MIN_PLAYERS

            random.shuffle(all_bots)
            bots = all_bots[:num_players]

        overrides = []
        for bot in bots:
            overrides.append('-o')
            overrides.append(bot['name'])
        raw_results = compare_bots._play_game(hlt_path,
                                              [bot['path'] for bot in bots],
                                              flags + overrides)
        results = json.loads(raw_results)
        with connect(db_path) as conn:
            add_match(conn, bots, results)

        output.output('Played {}/{} matches...'.format(i + 1, iterations),
                      progress=i + 1,
                      iterations=iterations,
                      results=results,
                      participants=bots)
    output.output('Done playing games.', progress=iterations, iterations=iterations)


def list_matches(conn):
    matches = conn.execute('select * from games').fetchall()
    result = []
    for match in matches:
        match = dict(match)
        match['participants'] = json.loads(match['participants'])
        match['results'] = json.loads(match['results'])
        result.append(match)
    return result


def get_rank_history(conn, bot_id):
    records = conn.execute('select datetime, rank, mu, sigma from rank_history where bot_id = ?', (bot_id,))
    return [dict(row) for row in records]


def main(args):
    if args.gym_mode == BOTS_MODE:

        def _prettyprint_bot(bot):
            return "\n".join([
                'Bot "{}" (ID {}, version {})'.format(bot['name'], bot['id'], bot['version']),
                'Rank {} (score={:.02f}, μ={:.02f}, σ={:.02f})'.format(bot['rank'], bot['mu'] - 3*bot['sigma'], bot['mu'], bot['sigma']),
                'Path: {}'.format(bot['path']),
                'Games Played: {}'.format(bot['games_played']),
            ])

        if args.bot_name:
            with connect(args.db_path) as conn:
                bots = list_bots(conn)
                for bot in bots:
                    if bot['name'] == args.bot_name:
                        output.output(_prettyprint_bot(bot),
                                      bot=bot,
                                      history=get_rank_history(conn, bot['id']))
                        break
                else:
                    output.error('Bot not found.')
                    sys.exit(1)
            return

        with connect(args.db_path) as conn:
            bots = list_bots(conn)
            output.print_list("Registered Bots:", bots, formatter=_prettyprint_bot)
    elif args.gym_mode == STATS_MODE:
        if args.query:
            with connect(args.db_path) as conn:
                results = list(conn.execute(args.query).fetchall())
                if not results:
                    output.output("No results.", results=results)
                    return

                # TODO: table output func
                keys = list(results[0].keys())
                for key in keys:
                    print('{:>20}'.format(key), end='|')
                print()
                for key in keys:
                    print('-' * 20, end='+')
                print()
                for index, row in enumerate(results):
                    for key in keys:
                        print('{:>20}'.format(row[key]), end='|')
                    print()
            return

        def _prettyprint_match(match):
            winner = [p for p in match['participants'] if p['id'] == match['winner']][0]
            return 'Match #{}: "{}" beat {}\nMap Size: {}x{}\nReplay: {}'.format(
                match['id'],
                winner['name'],
                ' '.join([ '"{}"'.format(bot['name'])
                           for bot in match['participants']
                           if bot['id'] != winner['id'] ]),
                match['results']['map_width'],
                match['results']['map_height'],
                match['results']['replay'],
            )

        with connect(args.db_path) as conn:
            matches = list_matches(conn)
            output.print_list("Games Played:", matches, formatter=_prettyprint_match)
    elif args.gym_mode == REGISTER_MODE:
        with connect(args.db_path) as conn:
            register_bot(conn, args.name, args.path)
    elif args.gym_mode == DEREGISTER_MODE:
        with connect(args.db_path) as conn:
            deregister_bot(conn, args.name)
    elif args.gym_mode == EVALUATE_MODE:
        hlt_path = args.halite_binary
        output_dir = args.game_output_dir
        iterations = args.iterations

        run_matches(args.db_path, hlt_path, output_dir, iterations)


def parse_arguments(subparser):
    gym_parser = subparser.add_parser('gym', help='Train your Bot(s)!')
    gym_parser.add_argument('--db-path',
                            dest='db_path',
                            action='store',
                            type=str,
                            required=False,
                            default=None,
                            help="Specify an alternate database file.")
    gym_subparser = gym_parser.add_subparsers(dest='gym_mode')

    register_parser = gym_subparser.add_parser(REGISTER_MODE, help='Register or update a bot with the gym.')
    register_parser.add_argument('name', type=str,
                                 help="The name of the bot to add/update.")
    register_parser.add_argument('path', type=str,
                                 help="The command to run to start the bot.")

    deregister_parser = gym_subparser.add_parser(DEREGISTER_MODE, help='Delete a bot.')
    deregister_parser.add_argument('name', type=str,
                                   help="The name of the bot to delete.")

    evaluate_parser = gym_subparser.add_parser(EVALUATE_MODE, help='Run games with bots in the gym.')
    evaluate_parser.add_argument('-b', '--binary',
                                 dest='halite_binary',
                                 action='store',
                                 type=str, required=True,
                                 help="The halite executable/binary path, used to run the games")
    evaluate_parser.add_argument('--output-dir',
                                 dest='game_output_dir',
                                 action='store',
                                 type=str, required=False,
                                 help="A path to a directory where logs and replays will be stored.")
    evaluate_parser.add_argument('-i', '--iterations',
                                 dest='iterations',
                                 action='store',
                                 type=int, required=False,
                                 default=10,
                                 help="Number of games to play.")

    stats_parser = gym_subparser.add_parser(STATS_MODE, help='Get stats from the gym.')
    stats_parser.add_argument('query', nargs='?', type=str,
                              help="An SQL query to run (this is NOT SANITIZED in any way!)")

    bots_parser = gym_subparser.add_parser(BOTS_MODE, help='List registered bots.')
    bots_parser.add_argument('bot_name', type=str,
                             nargs='?',
                             default=None,
                             help="The name of the bot to query.")
