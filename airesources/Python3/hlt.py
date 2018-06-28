#!/usr/bin/env python

import sys, copy

base_players = None # Need to remember this

class Cell(object):
    def __init__(self, _type = "n", production = 0, energy_factor = 0):
        self._type = _type
        self.production = production
        self.passable = self._type != 'o'
        self.energy_factor = energy_factor

    def __str__(self):
        return self._type + str(self.production)

    def __repr__(self):
        return str(self)

class Player(object):
    def __init__(self, factory):
        self.factory = factory
        self.energy = 0
        self.entities = {}

class Entity(object):
    def __init__(self, owner, energy):
        self.owner = owner
        self.energy = energy

    def __str__(self):
        return "(" + str(self.owner) + ", " + str(self.energy) + ")"

    def __repr__(self):
        return str(self)

def get_init():
    global base_players
    num_players, my_id = map(int, input().split())
    players = {}
    for _ in range(num_players):
        player, factory_x, factory_y = map(int, input().split())
        players[player] = Player((factory_x, factory_y))
    base_players = copy.copy(players)

    map_w, map_h = map(int, input().split())
    game_map = [[None for _ in range(map_w)] for _ in range(map_h)]

    for y in range(map_h):
        for x in range(map_w):
            line = input().split()
            _type = line[0]
            production = 0
            if _type != "f":
                production = int(line[1])
            game_map[y][x] = Cell(_type, production)

    return game_map, players, my_id

def send_init(name):
    print(name if len(name) > 0 else ' ')
    sys.stdout.flush()

def get_frame():
    turn_number = int(input())
    players = copy.copy(base_players)
    for _ in range(len(players)):
        player, num_entities, energy = map(int, input().split())
        players[player].energy = energy
        players[player].entities = {}
        for _ in range(num_entities):
            x, y, _, energy = map(int, input().split())
            players[player].entities[(x, y)] = Entity(player, energy)
    return turn_number, players

def send_frame(moves):
    print(" ".join(["m " + str(loc[0]) + " " + str(loc[1]) + " " + direction for loc, direction in moves.items()]))
    sys.stdout.flush()

# I'm not yet doing custom logging because Python's logging is
# quite easy & comprehensive already, so it's low-priority.