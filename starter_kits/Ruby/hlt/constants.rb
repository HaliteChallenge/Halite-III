# The constants representing the game variation being played.
# They come from game engine and changing them has no effect.
# They are strictly informational.

module Constants
  # Load constants from JSON given by the game engine.
  def self.load_constants(constants)
    # The cost to build a single ship.
    const_set("SHIP_COST",  constants['NEW_ENTITY_ENERGY_COST'])

    # The cost to build a dropoff.
    const_set("DROPOFF_COST",  constants['DROPOFF_COST'])

    # The maximum amount of halite a ship can carry.
    const_set("MAX_HALITE",  constants['MAX_ENERGY'])

    # The maximum number of turns a game can last. This reflects the fact
    # that smaller maps play for fewer turns.
    const_set("MAX_TURNS",  constants['MAX_TURNS'])

    # 1/EXTRACT_RATIO halite (truncated) is collected from a square per turn.
    const_set("EXTRACT_RATIO",  constants['EXTRACT_RATIO'])

    # 1/MOVE_COST_RATIO halite (truncated) is needed to move off a cell.
    const_set("MOVE_COST_RATIO",  constants['MOVE_COST_RATIO'])

    # Whether inspiration is enabled.
    const_set("INSPIRATION_ENABLED",  constants['INSPIRATION_ENABLED'])

    # A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent
    # ships are within this Manhattan distance.
    const_set("INSPIRATION_RADIUS",  constants['INSPIRATION_RADIUS'])

    # A ship is inspired if at least this many opponent ships are within
    # INSPIRATION_RADIUS distance.
    const_set("INSPIRATION_SHIP_COUNT",  constants['INSPIRATION_SHIP_COUNT'])

    # An inspired ship mines 1/X halite from a cell per turn instead.
    const_set("INSPIRED_EXTRACT_RATIO",  constants['INSPIRED_EXTRACT_RATIO'])

    # An inspired ship that removes Y halite from a cell collects X*Y additional halite.
    const_set("INSPIRED_BONUS_MULTIPLIER",  constants['INSPIRED_BONUS_MULTIPLIER'])

    # An inspired ship instead spends 1/X% halite to move.
    const_set("INSPIRED_MOVE_COST_RATIO",  constants['INSPIRED_MOVE_COST_RATIO'])
  end
end
