"""
The constants representing the game variation being played.
They come from game engine and changing them has no effect.
They are strictly informational.
"""
struct Constants
    "The cost to build a single ship."
    SHIP_COST::Int
    "The cost to build a dropoff."
    DROPOFF_COST::Int
    "The maximum amount of halite a ship can carry."
    MAX_HALITE::Int
    """ 
    The maximum number of turns a game can last. This reflects the fact
    that smaller maps play for fewer turns.
    """
    MAX_TURNS::Int
    "1/EXTRACT_RATIO halite (truncated) is collected from a square per turn."
    EXTRACT_RATIO::Int
    "1/MOVE_COST_RATIO halite (truncated) is needed to move off a cell."
    MOVE_COST_RATIO::Int
    "Whether inspiration is enabled."
    INSPIRATION_ENABLED::Bool
    """
    A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent
    ships are within this Manhattan distance.
    """
    INSPIRATION_RADIUS::Int
    """
    A ship is inspired if at least this many opponent ships are within
    INSPIRATION_RADIUS distance.
    """
    INSPIRATION_SHIP_COUNT::Int
    "An inspired ship mines 1/X halite from a cell per turn instead."
    INSPIRED_BONUS_MULTIPLIER::Float64
    "An inspired ship that removes Y halite from a cell collects X*Y additional halite."
    INSPIRED_EXTRACT_RATIO::Int
    "An inspired ship instead spends 1/X% halite to move."
    INSPIRED_MOVE_COST_RATIO::Int
end

"Load constants from JSON given by the game engine."
function load_constants(constants::Dict)
    SHIP_COST = constants["NEW_ENTITY_ENERGY_COST"]
    DROPOFF_COST = constants["DROPOFF_COST"]
    MAX_HALITE = constants["MAX_ENERGY"]
    MAX_TURNS = constants["MAX_TURNS"]
    EXTRACT_RATIO = constants["EXTRACT_RATIO"]
    MOVE_COST_RATIO = constants["MOVE_COST_RATIO"]
    INSPIRATION_ENABLED = constants["INSPIRATION_ENABLED"]
    INSPIRATION_RADIUS = constants["INSPIRATION_RADIUS"]
    INSPIRATION_SHIP_COUNT = constants["INSPIRATION_SHIP_COUNT"]
    INSPIRED_EXTRACT_RATIO = constants["INSPIRED_EXTRACT_RATIO"]
    INSPIRED_BONUS_MULTIPLIER = constants["INSPIRED_BONUS_MULTIPLIER"]
    INSPIRED_MOVE_COST_RATIO = constants["INSPIRED_MOVE_COST_RATIO"]

    Constants(SHIP_COST, DROPOFF_COST, MAX_HALITE, MAX_TURNS, EXTRACT_RATIO,
              MOVE_COST_RATIO, INSPIRATION_ENABLED, INSPIRATION_RADIUS,
              INSPIRATION_SHIP_COUNT, INSPIRED_BONUS_MULTIPLIER,
              INSPIRED_EXTRACT_RATIO, INSPIRED_MOVE_COST_RATIO)
end