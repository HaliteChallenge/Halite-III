"""
Global constants relating to the game engine.
"""

"""Maximum number of steps to consider in pathfinding."""
MAX_BFS_STEPS = 1024  # = can search a 32x32 area completely

"""The maximum amount of halite a ship can carry."""
MAX_HALITE = 1000
"""The cost to build a single ship."""
SHIP_COST = 500
"""The cost to build a dropoff."""
DROPOFF_COST = 2000
"""The maximum number of turns a game can last."""
MAX_TURNS = 500
"""1/EXTRACT_RATIO halite (rounded) is collected from a square per turn."""
EXTRACT_RATIO = 4
"""1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell."""
MOVE_COST_RATIO = 10
"""Whether the engine will terminate bots for nonsensical commands."""
STRICT_ERRORS = False

def load_constants(constants):
    """
    Load constants from JSON given by the game engine.
    """
    global SHIP_COST, DROPOFF_COST, MAX_HALITE, MAX_TURNS
    global EXTRACT_RATIO, MOVE_COST_RATIO, STRICT_ERRORS
    SHIP_COST = constants.get('NEW_ENTITY_ENERGY_COST', SHIP_COST)
    DROPOFF_COST = constants.get('DROPOFF_COST', DROPOFF_COST)
    MAX_HALITE = constants.get('MAX_ENERGY', MAX_HALITE)
    MAX_TURNS = constants.get('MAX_TURNS', MAX_TURNS)
    EXTRACT_RATIO = constants.get('EXTRACT_RATIO', EXTRACT_RATIO)
    MOVE_COST_RATIO = constants.get('MOVE_COST_RATIO', MOVE_COST_RATIO)
    STRICT_ERRORS = constants.get('STRICT_ERRORS', STRICT_ERRORS)
