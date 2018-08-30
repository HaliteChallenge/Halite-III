"""
Constants representing the game variation being played.
Most constants are global and come from game engine and are immutable and are strictly informational.
Some constants are only used by the local game client and so are mutable.
"""

################################################
# Local and mutable constants.

"""Maximum number of steps to consider in pathfinding."""
MAX_BFS_STEPS = 1024  # = can search a 32x32 area completely

################################################
# Global and immutable constants.

"""The maximum amount of halite a ship can carry."""
MAX_HALITE = 1000
"""The cost to build a single ship."""
SHIP_COST = 1000
"""The cost to build a dropoff."""
DROPOFF_COST = 4000
"""The maximum number of turns a game can last."""
MAX_TURNS = 500
"""1/EXTRACT_RATIO halite (rounded) is collected from a square per turn."""
EXTRACT_RATIO = 4
"""1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell."""
MOVE_COST_RATIO = 10
"""Whether inspiration is enabled."""
INSPIRATION_ENABLED = True
"""A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent ships are within this Manhattan distance."""
INSPIRATION_RADIUS = 4
"""A ship is inspired if at least this many opponent ships are within INSPIRATION_RADIUS distance."""
INSPIRATION_SHIP_COUNT = 2
"""An inspired ship mines 1/X halite from a cell per turn instead."""
INSPIRED_EXTRACT_RATIO = 4
"""An inspired ship that removes Y halite from a cell collects X*Y additional halite."""
INSPIRED_BONUS_MULTIPLIER = 2
"""An inspired ship instead spends 1/X% halite to move."""
INSPIRED_MOVE_COST_RATIO = 12
"""Whether capture is enabled."""
CAPTURE_ENABLED = True
"""A ship is captured if an opponent has CAPTURE_SHIP_ADVANTAGE more ships than you within this distance."""
CAPTURE_RADIUS = 3
"""A ship is captured if an opponent has this many more ships than you within CAPTURE_RADIUS distance."""
CAPTURE_SHIP_ADVANTAGE = 3

def load_constants(constants):
    """
    Load constants from JSON given by the game engine.
    """
    global SHIP_COST, DROPOFF_COST, MAX_HALITE, MAX_TURNS
    global EXTRACT_RATIO, MOVE_COST_RATIO
    global INSPIRATION_ENABLED, INSPIRATION_RADIUS, INSPIRATION_SHIP_COUNT
    global INSPIRED_EXTRACT_RATIO, INSPIRED_BONUS_MULTIPLIER, INSPIRED_MOVE_COST_RATIO
    global CAPTURE_ENABLED, CAPTURE_RADIUS, CAPTURE_SHIP_ADVANTAGE
    SHIP_COST = constants.get('NEW_ENTITY_ENERGY_COST', SHIP_COST)
    DROPOFF_COST = constants.get('DROPOFF_COST', DROPOFF_COST)
    MAX_HALITE = constants.get('MAX_ENERGY', MAX_HALITE)
    MAX_TURNS = constants.get('MAX_TURNS', MAX_TURNS)
    EXTRACT_RATIO = constants.get('EXTRACT_RATIO', EXTRACT_RATIO)
    INSPIRATION_ENABLED = constants.get('INSPIRATION_ENABLED', INSPIRATION_ENABLED)
    INSPIRATION_RADIUS = constants.get('INSPIRATION_RADIUS', INSPIRATION_RADIUS)
    INSPIRATION_SHIP_COUNT = constants.get('INSPIRATION_SHIP_COUNT', INSPIRATION_SHIP_COUNT)
    INSPIRED_EXTRACT_RATIO = constants.get('INSPIRED_EXTRACT_RATIO', INSPIRED_EXTRACT_RATIO)
    INSPIRED_BONUS_MULTIPLIER = constants.get('INSPIRED_BONUS_MULTIPLIER', INSPIRED_BONUS_MULTIPLIER)
    INSPIRED_MOVE_COST_RATIO = constants.get('INSPIRED_MOVE_COST_RATIO', INSPIRED_MOVE_COST_RATIO)
    CAPTURE_ENABLED = constants.get('CAPTURE_ENABLED', CAPTURE_ENABLED)
    CAPTURE_RADIUS = constants.get('CAPTURE_RADIUS', CAPTURE_RADIUS)
    CAPTURE_SHIP_ADVANTAGE = constants.get('CAPTURE_SHIP_ADVANTAGE', CAPTURE_SHIP_ADVANTAGE)
