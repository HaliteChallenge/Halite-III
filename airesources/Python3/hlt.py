#!/usr/bin/env python

import copy
import logging
import random
import sys

# Game constants for reference
MAX_HALITE = 1000
SHIP_COST = 500
DROPOFF_COST = 2000


# Store the most recent game map globally (would rather not, but
# easier than threading it everywhere)
_global_map = None


class Location:
    """
    A location utility class.
    """
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def distance(self, other):
        """
        Compute the Manhattan distance to the given point.

        Accounts for wraparound.
        """
        global _global_map
        return _global_map.distance(self, other)

    def with_offset(self, dx, dy):
        """
        Create a new Location with the given offset from this.

        Normalizes the coordinates afterwards.
        """
        global _global_map
        return _global_map.location_with_offset(self, dx, dy)

    def with_direction(self, direction):
        """
        Create a new Location one unit away in the given direction.

        Normalizes the coordinates afterwards.
        """
        global _global_map
        return _global_map.location_with_direction(self, direction)

    def normalized(self):
        """
        Create a copy of this location with the coordinates normalized.
        """
        global _global_map
        return _global_map.normalize(self)

    def towards(self, target):
        """
        Return the direction to move closer to the target point, or
        None if the points are the same.
        """
        global _global_map
        if self.x != target.x:
            if self.x > target.x:
                west_dist = self.x - target.x
                east_dist = _global_map.width - west_dist
            else:
                east_dist = target.x - self.x
                west_dist = _global_map.width - east_dist
            if west_dist < east_dist:
                return "w"
            return "e"
        elif self.y != target.y:
            if self.y > target.y:
                north_dist = self.y - target.y
                south_dist = _global_map.height - north_dist
            else:
                south_dist = self.y - target.y
                north_dist = _global_map.height - south_dist
            if north_dist < south_dist:
                return "n"
            return "s"

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    def __repr__(self):
        return "Location({}, {})".format(self.x, self.y)


class SafeMover:
    """
    Helper class that prevents your own ships from crashing into each
    other.

    To use, create a new instance on each turn. This class keeps track
    of where ships are and where your ships are moving, and changes
    your moves in case they conflict.

    This only offers basic collision avoidance: it cannot do things
    like swap the position of ships, and it isn't very cautious near
    other players' ships.

    """
    def __init__(self, game_map, players):
        self.game_map = game_map
        self.players = players
        # Map of ship ID to target location
        self.moves = {}

    def move(self, ship, direction, tries=5):
        """
        Move the given ship in the given direction, or try and move in
        a different direction if the original direction is unsafe to move in.

        We try to move in a different direction, instead of staying
        still, to avoid deadlocks, where two or more ships are trying
        to swap positions. (This is technically safe, but we will
        leave that up to you.) In such cases, moving randomly prevents
        ships from waiting for each other forever.
        """
        if tries <= 0:
            return None

        my_target = ship.location.with_direction(direction)
        for other_target in self.moves.values():
            if my_target == other_target:
                return self.move(ship, random.choice("nsew"), tries - 1)

        for player in self.players.values():
            for other_ship in player.ships.values():
                if other_ship.location == my_target:
                    return self.move(ship, random.choice("nsew"), tries - 1)

        self.moves[ship.id] = my_target
        return ship.move_unsafe(direction)

    def move_towards(self, ship, target):
        """
        Safely move this ship towards the given point.
        """
        return self.move(ship, ship.location.towards(target))


class Ship:
    def __init__(self, id, location, halite):
        self.id = id
        self.location = location
        self.halite = halite

    @property
    def is_full(self):
        """Is this ship at max capacity?"""
        return self.halite == MAX_HALITE

    def construct_dropoff(self):
        """Return a move to convert this ship into a dropoff."""
        return "c {}".format(self.id)

    def move_unsafe(self, direction):
        """
        Return a move to move this ship in a direction without
        checking for collisions.
        """
        return "m {} {}".format(self.id, direction)

    def dump(self, amount):
        """
        Return a move to dump halite from this ship.
        """
        return "d {} {}".format(self.id, amount)

    def __repr__(self):
        return "Ship(id={}, {}, cargo={} halite)".format(self.id, self.location, self.halite)


class Dropoff:
    def __init__(self, id, location):
        self.id = id
        self.location = location


class Shipyard(Dropoff):
    def spawn(self, energy):
        """Return a move to spawn a new ship."""
        return "g {}".format(energy)


class Game:
    def __init__(self):
        self.name = None
        self.my_id = None
        self.players = None
        self.base_players = None
        self.game_map = None

    def get_init(self):
        """
        Get the initial game state.

        :returns: The initial game map, list of players, and your player ID.
        """
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
            players[player] = Player(Shipyard(-1, Location(shipyard_x, shipyard_y)))
        self.base_players = copy.copy(players)
        self.players = players

        map_w, map_h = map(int, input().split())
        game_map = [[None for _ in range(map_w)] for _ in range(map_h)]
        for y in range(map_h):
            row_cells = [MapCell(int(cell)) for cell in input().split()]
            for x in range(map_w):
                game_map[y][x] = row_cells[x]

        self.game_map = GameMap(game_map, map_w, map_h)
        global _global_map
        _global_map = self.game_map

        return self.game_map, players, my_id

    def send_init(self, name):
        """
        Tell the engine that you are done initializing.

        :param name: The name of your bot. (Only used locally.)
        """
        self.name = name
        print(name)
        sys.stdout.flush()

    def get_frame(self):
        """
        Get the next turn's game state.

        :returns: The turn number, an updated map, and an updated player list.
        """
        turn_number = int(input())
        logging.info("=============== TURN {:03} ================".format(turn_number))
        self.players = copy.copy(self.base_players)

        for _ in range(len(self.players)):
            player, num_ships, num_dropoffs, halite = map(int, input().split())
            self.players[player].halite = halite
            self.players[player].ships = {}
            self.players[player].dropoffs = {}
            for _ in range(num_ships):
                ship_id, x, y, halite = map(int, input().split())
                self.players[player].ships[ship_id] = Ship(ship_id, Location(x, y), halite)
            for _ in range(num_dropoffs):
                dropoff_id, x, y = map(int, input().split())
                self.players[player].dropoffs[dropoff_id] = Dropoff(dropoff_id, Location(x, y))

        # Get changed cells
        num_changed_cells = int(input())
        for _ in range(num_changed_cells):
            cell_x, cell_y, cell_energy = map(int, input().split())
            self.game_map[cell_y][cell_x].halite = cell_energy

        global _global_map
        _global_map = self.game_map

        return turn_number, self.game_map, self.players

    def end_turn(self, commands):
        print(" ".join(filter(lambda x: x is not None, commands)))
        sys.stdout.flush()


class Player:
    def __init__(self, shipyard):
        self.shipyard = shipyard
        self.halite = 0
        self.ships = {}
        self.dropoffs = {}


class MapCell:
    """A cell on the game map."""
    def __init__(self, halite):
        self.halite = halite
        self.ships = {}


class GameMap:
    """
    The game map.

    Can be indexed by a location, or by row and then
    column. Coordinates start at 0. This class does not normalize
    coordinates for you.
    """
    def __init__(self, cells, width, height):
        self.width = width
        self.height = height
        self.cells = cells

    def __getitem__(self, row):
        if isinstance(row, Location):
            return self.cells[row.y][row.x]
        return self.cells[row]

    def distance(self, loc1, loc2):
        """Compute the Manhattan distance between two locations.

        Accounts for wrap-around.
        """
        x_dist = abs(loc1.x - loc2.x)
        y_dist = abs(loc1.y - loc2.y)
        return min(x_dist, self.width - x_dist) + \
            min(y_dist, self.height - y_dist)

    def location_with_offset(self, location, dx, dy):
        return self.normalize(Location(location.x + dx, location.y + dy))

    def location_with_direction(self, location, direction):
        dx = 0
        dy = 0

        if direction == "n":
            dy = -1
        elif direction == "s":
            dy = 1
        elif direction == "w":
            dx = -1
        elif direction == "e":
            dx = 1

        return self.location_with_offset(location, dx, dy)

    def normalize(self, location):
        return Location((location.x + self.width) % self.width,
                        (location.y + self.height) % self.height)
