#!/usr/bin/env python

import numpy
import sys
import random
import time

class Cell(object):
    def __init__(self, _type = "n", production = 0):
        self._type = _type
        self.production = production

    def __str__(self):
        return self._type + str(self.production)

    def __repr__(self):
        return str(self)

class Player(object):
    def __init__(self, factory):
        self.factory = factory
        self.energy = 0
        self.entities = []

class Entity(object):
    def __init__(self, location, owner, energy):
        self.location = location
        self.owner = owner
        self.energy = energy

    def __str__(self):
        return "(" + str(self.location) + ", " + str(self.owner) + ", " + str(self.energy) + ")"

    def __repr__(self):
        return str(self)

num_players, player_id = map(int, raw_input().split())
players = {}
for _ in xrange(num_players):
    player, factory_x, factory_y = map(int, raw_input().split())
    players[player] = Player((factory_x, factory_y))

map_w, map_h = map(int, raw_input().split())
grid = [[None for _ in xrange(map_w)] for _ in xrange(map_h)]

for y in xrange(map_h):
    for x in xrange(map_w):
        line = raw_input().split()
        _type = line[0]
        production = 0
        if _type != "f":
            production = int(line[1])
        grid[y][x] = Cell(_type, production)

turn_number = 0

def select(entities):
    entity = random.choice(entities)
    entities.remove(entity)
    direction = random.choice(["n", "e", "s", "w"])
    return "m " + str(entity.location[0]) + " " + str(entity.location[1]) + " " + direction

def run_turn():
    turn_number = int(raw_input())
    for _ in xrange(len(players)):
        player, entities, energy = map(int, raw_input().split())
        players[player].energy = energy
        players[player].entities = []
        for _ in xrange(entities):
            x, y, _, energy = map(int, raw_input().split())
            players[player].entities.append(Entity((x, y), player, energy))
    print " ".join((select(players[player_id].entities) for _ in xrange(len(players[player_id].entities))))
    # Sleep to test slow bots
    time.sleep(random.uniform(0.0, 0.1))
    sys.stdout.flush()

print "player" + str(player_id)
sys.stdout.flush()
while True:
    run_turn()
