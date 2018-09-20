#!/usr/bin/env python3

import argparse
import collections
import json
import sys
import zstd


def read_replay(filelike):
    return json.loads(zstd.loads(filelike.read()))


def nightmare(replay, player_id):
    # import code; code.interact(local=locals())

    if player_id >= replay['number_of_players'] or player_id < 0:
        raise ValueError('Player ID {} is invalid for a {}-player game.'.format(
            player_id, replay['number_of_players']
        ))

    ## Game header
    yield json.dumps(replay['GAME_CONSTANTS'])
    yield '{} {}'.format(replay['number_of_players'], player_id)

    ## Initial player state
    for player in sorted(replay['players'], key=lambda player: player['player_id']):
        yield '{} {} {}'.format(player['player_id'],
                                player['factory_location']['x'],
                                player['factory_location']['y'])

    ## Initial map state
    yield '{} {}'.format(replay['production_map']['width'],
                         replay['production_map']['height'])
    for row in replay['production_map']['grid']:
        yield ' '.join(str(cell['energy']) for cell in row)

    ## Each frame
    dropoffs = collections.defaultdict(list)
    for index, frame in enumerate(replay['full_frames']):
        yield str(index + 1)

        for player_index in range(replay['number_of_players']):
            yield '{} {} {} {}'.format(
                player_index,
                len(frame['entities'].get(str(player_index), {})),
                len(dropoffs[player_index]),
                frame['energy'][str(player_index)])

            for entity_id, entity in frame['entities'].get(str(player_index), {}).items():
                yield '{} {} {} {}'.format(entity_id, entity['x'], entity['y'], entity['energy'])

            for dropoff in dropoffs[player_index]:
                yield '{} {} {}'.format(dropoff['id'], dropoff['location']['x'], dropoff['location']['y'])

        for cell in frame['cells']:
            yield '{} {} {}'.format(cell['x'], cell['y'], cell['production'])

        for event in frame['events']:
            if event['type'] == 'construct':
                dropoffs[event['owner_id']].append(event)


def main():
    parser = argparse.ArgumentParser(description='Turn replay files into bot input to force a bot to replay a game.')
    parser.add_argument('-i', '--input',
                        help='Replay file to read (or - for STDIN)')
    parser.add_argument('-o', '--output',
                        help='Output file for bot messages (or - for STDOUT)')
    parser.add_argument('-p', '--player', default=0, type=int,
                        help='Which player to replay (default 0)')
    args = parser.parse_args()

    infile = None
    outfile = None

    if not args.input or args.input == '-':
        infile = sys.stdin.buffer
    else:
        infile = open(args.input, 'rb')

    if not args.output or args.output == '-':
        outfile = sys.stdout
    else:
        outfile = open(args.output, 'w')

    with outfile:
        with infile:
            replay = read_replay(infile)

        for line in nightmare(replay, args.player):
            outfile.write(line)
            outfile.write('\n')


if __name__ == '__main__':
    main()
