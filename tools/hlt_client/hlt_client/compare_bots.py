import json
import os
import subprocess

from . import output

_SPACE_DELIMITER = ' '
_BOT_ID_POSITION = 1


def _determine_winner(results):
    """
    From the game result string, extract the winner's id.
    :param game_result: The result of running a game on the Halite binary
    :return:
    """
    for player_id, stats in results["stats"].items():
        if stats["rank"] == 1:
            return player_id


def _play_game(binary, bot_commands, flags):
    """
    Plays one game considering the specified bots and the game and map constraints.
    :param binary: The halite binary
    :param bot_commands: The commands to run each of the bots
    :return: The game's result string
    """
    command = [
        binary,
        "--results-as-json"
    ]
    command.extend(flags)
    for bot_command in bot_commands:
        command.append(bot_command)
    return subprocess.check_output(command).decode()


def play_games(binary, game_output_dir, map_width, map_height, bot_commands, number_of_runs, flags):
    """
    Runs number_of_runs games using the designated bots and binary, recording the tally of wins per player
    :param binary: The Halite binary.
    :param game_output_dir: Where to put replays and log files.
    :param map_width: The map width, set to None for engine random choice
    :param map_height: The map height, set to None for engine random choice
    :param bot_commands: The commands to run each of the bots (must be either 2 or 4)
    :param number_of_runs: How many runs total
    :return: Nothing
    """

    binary = os.path.abspath(binary)

    if game_output_dir is not None:
        game_output_dir = os.path.abspath(game_output_dir)
        if not os.path.lexists(game_output_dir):
            try:
                os.mkdir(game_output_dir)
            except FileExistsError:
                pass

        flags = flags + ['-i', game_output_dir]

    if map_width is not None:
        flags.extend(["--width", str(map_width)])
    if map_height is not None:
        flags.extend(["--height", str(map_height)])

    output.output("Comparing Bots!")
    result = {}
    if not(len(bot_commands) == 4 or len(bot_commands) == 2):
        raise IndexError("The number of bots specified must be either 2 or 4.")
    for current_run in range(0, number_of_runs):
        match_output = _play_game(binary, bot_commands, flags)
        results = json.loads(match_output)
        winner = _determine_winner(results)
        result[winner] = result.setdefault(winner, 0) + 1
        output.output("Finished {} runs.".format(current_run + 1), games_played=current_run + 1)
        output.output("Win Ratio: {}".format(result), stats=result, results=results)


def parse_arguments(subparser):
    bot_parser = subparser.add_parser('play', help='Play games using your bot(s).')
    bot_parser.add_argument('-r', '--run-command',
                            dest='run_commands',
                            action='append',
                            type=str, required=True,
                            help="The command to run a specific bot. You may pass either 2 or 4 of these arguments")
    bot_parser.add_argument('-b', '--binary',
                            dest='halite_binary',
                            action='store',
                            type=str, required=True,
                            help="The halite game engine path, used to run the games. Included in starter kits/from the download page.")

    bot_parser.add_argument('--output-dir',
                            dest='game_output_dir',
                            action='store',
                            type=str, required=False,
                            help="A path to a directory where logs and replays will be stored. If provided, use absolute paths in any bot commands.")

    bot_parser.add_argument('-W', '--width',
                            dest='map_width',
                            action='store',
                            type=int, default=None,
                            help="The map width the simulations will run in")
    bot_parser.add_argument('-H', '--height',
                            dest='map_height',
                            action='store',
                            type=int, default=None,
                            help="The map height the simulations will run in")
    bot_parser.add_argument('-i', '--iterations',
                            dest='iterations',
                            action='store',
                            type=int,  default=100,
                            help="Number of games to be run")
