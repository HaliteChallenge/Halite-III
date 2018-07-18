#!/usr/bin/env python

import sys, copy
import logging
from collections import namedtuple

MAX_HALITE = 1000
SHIP_COST = 1000

Ship = namedtuple('Ship', ['id', 'location', 'halite'])
Dropoff = namedtuple('Ship', ['id', 'location'])


class Bot:
    def __init__(self):
        self.name = None
        self.my_id = None
        self.players = None
        self.base_players = None
        self.game_map = None

    def get_init(self):
        num_players, my_id = map(int, input().split())

        self.my_id = my_id

        logging.basicConfig(
            filename="bot-{}.log".format(my_id),
            filemode="w",
            level=logging.DEBUG,
        )

        players = {}
        for _ in range(num_players):
            player, shipyard_x, shipyard_y = map(int, input().split())
            players[player] = Player((shipyard_x, shipyard_y))
        self.base_players = copy.copy(players)
        self.players = players

        map_w, map_h = map(int, input().split())
        game_map = [[None for _ in range(map_w)] for _ in range(map_h)]
        for y in range(map_h):
            row_cells = [int(cell) for cell in input().split()]
            for x in range(map_w):
                game_map[y][x] = row_cells[x]

        self.game_map = GameMap(game_map, map_w, map_h)

        return game_map, players, my_id

    def send_init(self, name):
        self.name = name
        print(name)
        sys.stdout.flush()

    def get_frame(self):
        turn_number = int(input())
        self.players = copy.copy(self.base_players)

        for _ in range(len(self.players)):
            player, num_ships, num_dropoffs, halite = map(int, input().split())
            self.players[player].halite = halite
            self.players[player].ships = {}
            self.players[player].dropoffs = {}
            for _ in range(num_ships):
                ship_id, x, y, halite = map(int, input().split())
                self.players[player].ships[ship_id] = Ship(ship_id, (x, y), halite)
            for _ in range(num_dropoffs):
                dropoff_id, x, y = map(int, input().split())
                self.players[player].dropoffs[dropoff_id] = Dropoff(dropoff_id, (x, y))

        # Get changed cells
        num_changed_cells = int(input())
        for _ in range(num_changed_cells):
            cell_x, cell_y, cell_energy = map(int, input().split())
            self.game_map[cell_y][cell_x] = cell_energy

        return turn_number, self.game_map, self.players, MoveSet()

    def end_turn(self, commands):
        commands.send()


class Player:
    def __init__(self, shipyard):
        self.shipyard = shipyard
        self.halite = 0
        self.ships = {}
        self.dropoffs = {}


class GameMap:
    def __init__(self, cells, width, height):
        self.width = width
        self.height = height
        self.cells = cells

    def __getitem__(self, row):
        return self.cells[row]


class MoveSet:
    def __init__(self):
        self.moves = []

    def move(self, ship_id, d):
        self.moves.extend(["m", str(ship_id), d])

    def spawn(self, halite):
        self.moves.extend(["g", str(halite)])

    def dump(self, ship_id, halite):
        self.moves.extend(["d", str(ship_id), str(halite)])

    def construct(self, ship_id):
        self.moves.extend(["c", str(ship_id)])

    def send(self):
        print(" ".join(self.moves))
        sys.stdout.flush()

# I'm not yet doing custom logging because Python's logging is
# quite easy & comprehensive already, so it's low-priority.
