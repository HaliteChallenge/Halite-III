#!/usr/bin/env python

import sys, copy

MAX_HALITE = 65535
SHIP_COST = 65536

base_players = None # Need to remember this
_moves = ""

class Player(object):
    def __init__(self, shipyard):
        self.shipyard = shipyard
        self.halite = 0
        self.ships = {}
        self.dropoffs = set()

def get_init():
    global base_players
    num_players, my_id = map(int, input().split())
    players = {}
    for _ in range(num_players):
        player, shipyard_x, shipyard_x = map(int, input().split())
        players[player] = Player((shipyard_x, shipyard_x))
    base_players = copy.copy(players)

    map_w, map_h = map(int, input().split())
    game_map = [[None for _ in range(map_w)] for _ in range(map_h)]

    for y in range(map_h):
        for x in range(map_w):
            game_map[y][x] = int(input())

    return game_map, players, my_id

def send_init(name):
    print(name if len(name) > 0 else ' ')
    sys.stdout.flush()

def get_frame():
    turn_number = int(input())
    players = copy.copy(base_players)
    for _ in range(len(players)):
        player, num_ships, num_dropoffs, halite = map(int, input().split())
        players[player].halite = halite
        players[player].ships = {}
        players[player].dropoffs = set()
        for _ in range(num_ships):
            x, y, halite = map(int, input().split())
            players[player].ships[(x, y)] = halite
        for _ in range(num_dropoffs):
            x, y, halite = map(int, input().split())
            players[player].dropoffs.add((x, y))
    return turn_number, players

def move(loc, d):
    global _moves
    _moves += "m " + str(loc[0]) + ' ' + str(loc[1]) + ' ' + d + ' '
def spawn(halite):
    global _moves
    _moves += "g " + str(halite) + ' '
def dump(loc, halite):
    global _moves
    _moves += "d " + str(loc[0]) + ' ' + str(loc[1]) + ' ' + str(halite) + ' '
def construct(loc):
    global _moves
    _moves += "c " + str(loc[0]) + ' ' + str(loc[1]) + ' '
def send_frame():
    global _moves
    print(_moves)
    sys.stdout.flush()
    _moves = ""

# I'm not yet doing custom logging because Python's logging is
# quite easy & comprehensive already, so it's low-priority.
