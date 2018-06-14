#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "nlohmann/json_fwd.hpp"

namespace hlt {

/** The type of dimension values across all halite classes using dimension. */
using dimension_type = long;

/**
 * Gameplay constants that may be tweaked, though they should be at their
 * default values in a tournament setting.
 *
 * Supports serialization to and deserialization from JSON.
 */
struct GameConstants {
    unsigned long MAX_TURNS = 300;           /**< The maximum number of turns. */
    unsigned long MAX_PLAYERS = 16;          /**< The maximum number of players. */
    hlt::dimension_type DEFAULT_MAP_WIDTH = 128;   /**< The default width of generated maps. */
    hlt::dimension_type DEFAULT_MAP_HEIGHT = 128;  /**< The default height of generated maps. */

    unsigned long MAX_CELL_PRODUCTION = 255; /**< The maximum amount of production on a cell. */
    unsigned long MIN_CELL_PRODUCTION = 1;   /**< The minimum amount of production on a cell. */
    unsigned long MAX_ENERGY = 255;          /**< The maximum amount of energy per entity. */
    double BLUR_FACTOR = 0.75;               /**< The blur factor for the blurred tile generator. */

    /**
     * Get a mutable reference to the singleton constants.
     * @return Mutable reference to the singleton constants.
     */
    static GameConstants &get_mut() {
        // Guaranteed initialized only once by C++11
        static GameConstants instance;
        return instance;
    }

    /**
     * Get the singleton constants.
     * @return The singleton constants.
     */
    static const GameConstants &get() { return get_mut(); }

    /**
     * Encode the constants to JSON.
     * @param[out] json The JSON output.
     * @param constants The constants.
     */
    friend void to_json(nlohmann::json &json, const GameConstants &constants);

    /**
     * Decode the constants from JSON.
     * @param json The JSON input.
     * @param[out] constants The decoded constants.
     */
    friend void from_json(const nlohmann::json &json, GameConstants &constants);

    /** Delete the copy constructor. */
    GameConstants(const GameConstants &) = delete;

private:
    /** Hide the default constructor. */
    GameConstants() = default;
};

}

#endif // CONSTANTS_HPP
