package hlt

object Constants {
  /** The maximum amount of halite a ship can carry. */
  var MAX_HALITE = 0
  /** The cost to build a single ship. */
  var SHIP_COST = 0
  /** The cost to build a dropoff. */
  var DROPOFF_COST = 0
  /** The maximum number of turns a game can last. */
  var MAX_TURNS = 0
  /** 1/EXTRACT_RATIO halite (rounded) is collected from a square per turn. */
  var EXTRACT_RATIO = 0
  /** 1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell. */
  var MOVE_COST_RATIO = 0
  /** Whether inspiration is enabled. */
  var INSPIRATION_ENABLED = false
  /** A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent ships are within this Manhattan distance. */
  var INSPIRATION_RADIUS = 0
  /** A ship is inspired if at least this many opponent ships are within INSPIRATION_RADIUS distance. */
  var INSPIRATION_SHIP_COUNT = 0
  /** An inspired ship mines 1/X halite from a cell per turn instead. */
  var INSPIRED_EXTRACT_RATIO = 0
  /** An inspired ship that removes Y halite from a cell collects X*Y additional halite. */
  var INSPIRED_BONUS_MULTIPLIER = .0
  /** An inspired ship instead spends 1/X% halite to move. */
  var INSPIRED_MOVE_COST_RATIO = 0

  def populateConstants(stringFromEngine: String) = {
    val rawTokens = stringFromEngine.split("[{}, :\"]+")
    val tokens = rawTokens.filter(tok => !tok.isEmpty)

    if ((tokens.length % 2) != 0) {
      Log.log("Error: constants: expected even total number of key and value tokens from server.")
      throw new IllegalStateException
    }
    val constantsMap: Map[String, String] =
      (0 to tokens.length - 2)
        .filter(i => i % 2 == 0)
        .map(i => tokens(i) -> tokens(i + 1))
        .toMap

    def getInt(key: String): Int = {
      try {
        if (constantsMap.contains(key))
          constantsMap(key).toInt
        else
          throw new IllegalStateException(s"Error: constants: server did not send $key constant.")
      }
      catch {
        case err: NumberFormatException => {
          Log.log(s"Error: constants: $key constant has value of '${constantsMap(key)}' from server. Do not know how to parse that as int.")
          throw err
        }
      }
    }

    def getDouble(key: String): Double = {
      try {
        if (constantsMap.contains(key))
          constantsMap(key).toDouble
        else
          throw new IllegalStateException(s"Error: constants: server did not send $key constant.")
      }
      catch {
        case err: NumberFormatException => {
          Log.log(s"Error: constants: $key constant has value of '${constantsMap(key)}' from server. Do not know how to parse that as double.")
          throw err
        }
      }
    }

    def getBoolean(key: String): Boolean = {
      try {
        if (constantsMap.contains(key))
          constantsMap(key).toBoolean
        else
          throw new IllegalStateException(s"Error: constants: server did not send $key constant.")
      }
      catch {
        case err: IllegalArgumentException => {
          Log.log(s"Error: constants: $key constant has value of '${constantsMap(key)}' from server. Do not know how to parse that as boolean.")
          throw err
        }
      }
    }

    SHIP_COST = getInt("NEW_ENTITY_ENERGY_COST")
    DROPOFF_COST = getInt("DROPOFF_COST")
    MAX_HALITE = getInt("MAX_ENERGY")
    MAX_TURNS = getInt("MAX_TURNS")
    EXTRACT_RATIO = getInt("EXTRACT_RATIO")
    MOVE_COST_RATIO = getInt("MOVE_COST_RATIO")
    INSPIRATION_ENABLED = getBoolean("INSPIRATION_ENABLED")
    INSPIRATION_RADIUS = getInt("INSPIRATION_RADIUS")
    INSPIRATION_SHIP_COUNT = getInt("INSPIRATION_SHIP_COUNT")
    INSPIRED_EXTRACT_RATIO = getInt("INSPIRED_EXTRACT_RATIO")
    INSPIRED_BONUS_MULTIPLIER = getDouble("INSPIRED_BONUS_MULTIPLIER")
    INSPIRED_MOVE_COST_RATIO = getInt("INSPIRED_MOVE_COST_RATIO")
  }
}
