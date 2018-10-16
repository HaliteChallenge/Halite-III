package hlt

import java.util.*

object Constants {
    /** The maximum amount of halite a ship can carry.  */
    var MAX_HALITE: Int = 0
    /** The cost to build a single ship.  */
    var SHIP_COST: Int = 0
    /** The cost to build a dropoff.  */
    var DROPOFF_COST: Int = 0
    /** The maximum number of turns a game can last.  */
    var MAX_TURNS: Int = 0
    /** 1/EXTRACT_RATIO halite (rounded) is collected from a square per turn.  */
    var EXTRACT_RATIO: Int = 0
    /** 1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell.  */
    var MOVE_COST_RATIO: Int = 0
    /** Whether inspiration is enabled.  */
    var INSPIRATION_ENABLED: Boolean = false
    /** A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent ships are within this Manhattan distance.  */
    var INSPIRATION_RADIUS: Int = 0
    /** A ship is inspired if at least this many opponent ships are within INSPIRATION_RADIUS distance.  */
    var INSPIRATION_SHIP_COUNT: Int = 0
    /** An inspired ship mines 1/X halite from a cell per turn instead.  */
    var INSPIRED_EXTRACT_RATIO: Int = 0
    /** An inspired ship that removes Y halite from a cell collects X*Y additional halite.  */
    var INSPIRED_BONUS_MULTIPLIER: Double? = 0.toDouble()
    /** An inspired ship instead spends 1/X% halite to move.  */
    var INSPIRED_MOVE_COST_RATIO: Int = 0

    fun populateConstants(stringFromEngine: String?) {
        val rawTokens = stringFromEngine?.split("[{}, :\"]+".toRegex()) ?: ArrayList()
        val tokens = ArrayList<String>()
        for (i in rawTokens.indices) {
            if (!rawTokens[i].isEmpty()) {
                tokens.add(rawTokens[i])
            }
        }

        Log.log(tokens.toString())
        if (tokens.size % 2 != 0) {
            Log.log("Error: constants: expected even total number of key and value tokens from server.")
            throw IllegalStateException()
        }

        val constantsMap = HashMap<String, String>()

        var i = 0
        while (i < tokens.size) {
            constantsMap[tokens[i]] = tokens[i + 1]
            i += 2
        }

        SHIP_COST = getInt(constantsMap, "NEW_ENTITY_ENERGY_COST")
        DROPOFF_COST = getInt(constantsMap, "DROPOFF_COST")
        MAX_HALITE = getInt(constantsMap, "MAX_ENERGY")
        MAX_TURNS = getInt(constantsMap, "MAX_TURNS")
        EXTRACT_RATIO = getInt(constantsMap, "EXTRACT_RATIO")
        MOVE_COST_RATIO = getInt(constantsMap, "MOVE_COST_RATIO")
        INSPIRATION_ENABLED = getBoolean(constantsMap, "INSPIRATION_ENABLED")
        INSPIRATION_RADIUS = getInt(constantsMap, "INSPIRATION_RADIUS")
        INSPIRATION_SHIP_COUNT = getInt(constantsMap, "INSPIRATION_SHIP_COUNT")
        INSPIRED_EXTRACT_RATIO = getInt(constantsMap, "INSPIRED_EXTRACT_RATIO")
        INSPIRED_BONUS_MULTIPLIER = getDouble(constantsMap, "INSPIRED_BONUS_MULTIPLIER") ?: 0.0
        INSPIRED_MOVE_COST_RATIO = getInt(constantsMap, "INSPIRED_MOVE_COST_RATIO")
    }

    private fun getInt(map: Map<String, String>, key: String): Int {
        return Integer.parseInt(getString(map, key))
    }

    private fun getDouble(map: Map<String, String>, key: String): Double? {
        return getString(map, key)?.toDouble()
    }

    private fun getBoolean(map: Map<String, String>, key: String): Boolean {
        val stringValue = getString(map, key)
        return when (stringValue) {
            "true" -> true
            "false" -> false
            else -> {
                Log.log("Error: constants: " + key + " constant has value of '" + stringValue +
                        "' from server. Do not know how to parse that as boolean.")
                throw IllegalStateException()
            }
        }
    }

    private fun getString(map: Map<String, String>, key: String): String? {
        if (!map.containsKey(key)) {
            Log.log("Error: constants: server did not send $key constant.")
            throw IllegalStateException()
        }
        return map[key]
    }
}
