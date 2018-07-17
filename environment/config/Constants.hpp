#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "Units.hpp"

#include "nlohmann/json_fwd.hpp"

namespace hlt {

/**
 * Gameplay constants that may be tweaked, though they should be at their
 * default values in a tournament setting.
 *
 * Supports serialization to and deserialization from JSON.
 */
struct Constants {
    unsigned long MAX_TURNS = 300;              /**< The maximum number of turns. */
    unsigned long MAX_PLAYERS = 16;             /**< The maximum number of players. */
    dimension_type DEFAULT_MAP_WIDTH = 128;     /**< The default width of generated maps. */
    dimension_type DEFAULT_MAP_HEIGHT = 128;    /**< The default height of generated maps. */

    energy_type MAX_CELL_PRODUCTION = 100;      /**< The maximum maximum amount of production on a cell. */
    energy_type MIN_CELL_PRODUCTION = 40;       /**< The minimum maximum amount of production on a cell. */
    energy_type MAX_ENERGY = 255;               /**< The maximum amount of energy per entity. */
    energy_type NEW_ENTITY_ENERGY_COST = 1000;  /**< The amount of production which equals one energy. */
    // TODO: split this cost into the fixed and variable portions
    energy_type NEW_ENTITY_ENERGY = 255;        /**< The starting energy of a new entity. */
    energy_type BASE_TURN_ENERGY_LOSS = 1;      /**< The base energy decrease of an entity per turn. */
    energy_type INITIAL_ENERGY = 255000;        /**< The initial amount of energy for a player. */

    energy_type DROPOFF_COST = 1000;            /**< The cost of a dropoff construction. */
    unsigned long MOVE_COST_RATIO = 10;         /**< The cost of a move is the source's energy divided by this. */
    unsigned long DROPOFF_PENALTY_RATIO = 10;   /**< The cost ratio of using another player's dropoff. */
    unsigned long EXTRACT_RATIO = 10;           /**< The ratio of energy extracted from a cell per turn. */
    // TODO: ship max capacity

    double PERSISTENCE = 0.7; // Determines relative weight of local vs global features.
    double FACTOR_EXP_1 = 1.5; // Determines initial spikiness of map. Higher values weight towards 0.
    double FACTOR_EXP_2 = 1.5; // Determines final spikiness of map. Higher values weight towards 0.
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
     * Get a mutable reference to the singleton constants.
     * @return Mutable reference to the singleton constants.
     */
    static Constants &get_mut() {
        // Guaranteed initialized only once by C++11
        static Constants instance;
        return instance;
    }

    /**
     * Get the singleton constants.
     * @return The singleton constants.
     */
    static const Constants &get() { return get_mut(); }

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
