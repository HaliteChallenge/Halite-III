#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "nlohmann/json_fwd.hpp"

namespace hlt {

/** The type of dimension values across all Halite classes using dimension. */
using dimension_type = long;

/** The type of energy/production values across all Halite classes. */
using energy_type = long;

 /** Type of Player IDs. */
 using id_type = long;

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

    energy_type MAX_CELL_PRODUCTION = 255;      /**< The maximum amount of production on a cell. */
    energy_type MIN_CELL_PRODUCTION = 1;        /**< The minimum amount of production on a cell. */
    energy_type MAX_ENERGY = 255;               /**< The maximum amount of energy per entity. */
    energy_type NEW_ENTITY_ENERGY_COST = 1000;  /**< The amount of energy it takes to spawn a new entity. */
    energy_type NEW_ENTITY_ENERGY = 255;        /**< The starting energy of a new entity. */
    energy_type BASE_TURN_ENERGY_LOSS = 5;      /**< The base energy decrease of an entity per turn. */

    double BLUR_FACTOR = 0.75;                  /**< The blur factor for the blurred tile generator. */

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
