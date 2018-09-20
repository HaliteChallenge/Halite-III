#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "Units.hpp"

#include "nlohmann/json_fwd.hpp"

int main(int argc, char *argv[]);

namespace hlt {

/**
 * Gameplay constants that may be tweaked, though they should be at their
 * default values in a tournament setting.
 *
 * Supports serialization to and deserialization from JSON.
 */
struct Constants {
    friend int ::main(int argc, char *argv[]);

    bool STRICT_ERRORS = false;                 /**< Whether strict error checking mode is enabled. */

    unsigned long MAX_PLAYERS = 16;             /**< The maximum number of players. */
    dimension_type DEFAULT_MAP_WIDTH = 48;      /**< The default width of generated maps. */
    dimension_type DEFAULT_MAP_HEIGHT = 48;     /**< The default height of generated maps. */

    energy_type MAX_CELL_PRODUCTION = 1000;     /**< The maximum maximum amount of production on a cell. */
    energy_type MIN_CELL_PRODUCTION = 900;      /**< The minimum maximum amount of production on a cell. */
    energy_type MAX_ENERGY = 1000;              /**< The maximum amount of energy per entity. */
    energy_type NEW_ENTITY_ENERGY_COST = 1000;  /**< The base cost of a new entity. */
    energy_type INITIAL_ENERGY = 5000;          /**< The initial amount of energy for a player. */

    energy_type DROPOFF_COST = 4000;            /**< The cost of a dropoff construction. */
    unsigned long MOVE_COST_RATIO = 10;         /**< The cost of a move is the source's energy divided by this. */
    unsigned long DROPOFF_PENALTY_RATIO = 4;    /**< The cost ratio of using another player's dropoff. */
    unsigned long EXTRACT_RATIO = 4;            /**< The ratio of energy extracted from a cell per turn. */

    double PERSISTENCE = 0.7; // Determines relative weight of local vs global features.
    double FACTOR_EXP_1 = 2; // Determines initial spikiness of map. Higher values weight towards 0.
    double FACTOR_EXP_2 = 2; // Determines final spikiness of map. Higher values weight towards 0.

    unsigned long MIN_TURNS = 400;
    unsigned long MIN_TURN_THRESHOLD = 32;
    unsigned long MAX_TURNS = 500;
    unsigned long MAX_TURN_THRESHOLD = 64;

    /** Capture */
    bool CAPTURE_ENABLED = false; /**< whether to use capture */
    dimension_type CAPTURE_RADIUS = 3; /**< The distance in which a ship is considered for the capture calculation */
    unsigned long SHIPS_ABOVE_FOR_CAPTURE = 3; /**< If enemyships - friendlyships is above or equal to this threshold,
                                                        the ship is captured*/

    /** Inspiration **/
    bool INSPIRATION_ENABLED = true; /**< whether to use inspiration **/
    unsigned long INSPIRED_EXTRACT_RATIO = EXTRACT_RATIO; /**< alternative mining ratio for inspired ships */
    double INSPIRED_BONUS_MULTIPLIER = 2; /**< The benefit ratio of mining when inspired. (Removing Y halite from a cell gives you X*Y additional halite.) */
    unsigned long INSPIRED_MOVE_COST_RATIO = MOVE_COST_RATIO; /**< Alternative move cost ratio for inspired ships. */
    dimension_type INSPIRATION_RADIUS = 4; /** Maximum distance away for ships to count towards inspiration. */
    unsigned long INSPIRATION_SHIP_COUNT = 2; /**< If there are at least X enemy ships, then you are inspired. */

    /*
    The two FACTOR_EXP constants do related things but they are not the same.
    FACTOR_EXP_1 exponentiates the distribution used to seed the randomness.
    FACTOR_EXP_2 exponentiates the final distribution just prior to normalization.
    Broadly, both will give spikier maps. However (and perhaps counterintuitively), using
    FACTOR_EXP_1 will give maps that have more individual, small-scale spikes. Conversely,
    FACTOR_EXP_2 gives maps that moreso utilize the global structure, and have less noise.
    FACTOR_EXP_2 is also more sensitive than FACTOR_EXP_1.
    */

    static constexpr double BLUR_FACTOR = 0.75; // Not part of canon, needed to compile

    /**
     * Get the singleton constants.
     * @return The singleton constants.
     */
    static const Constants &get() { return get_mut(); }

    /**
     * Get a mutable reference to the singleton constants.
     * @return Mutable reference to the singleton constants.
     */
    static Constants &get_mut() {
        // Guaranteed initialized only once by C++11
        static Constants instance;
        return instance;
    }

    /**
     * Encode the constants to JSON.
     * @param[out] json The JSON output.
     * @param constants The constants.
     */
    friend void to_json(nlohmann::json &json, const Constants &constants);

    /**
     * Decode the constants from JSON.
     * @param json The JSON input.
     * @param[out] constants The decoded constants.
     */
    friend void from_json(const nlohmann::json &json, Constants &constants);

    /** Delete the copy constructor. */
    Constants(const Constants &) = delete;

private:
    /** Hide the default constructor. */
    Constants() = default;
};

}

#endif // CONSTANTS_HPP
