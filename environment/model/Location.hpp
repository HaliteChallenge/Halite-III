#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "nlohmann/json_fwd.hpp"
#include "Constants.hpp"

namespace hlt {

/** The four cardinal directions. */
enum class Direction : char {
    North = 'n',
    South = 's',
    East = 'e',
    West = 'w',
};

/**
 * Convert a Direction to JSON format.
 * @param[out] json The output JSON.
 * @param direction The Direction to convert.
 */
void to_json(nlohmann::json &json, const Direction &direction);

/**
 * Convert an encoded Direction from JSON format.
 * @param json The JSON.
 * @param[out] direction The converted Direction.
 */
void from_json(const nlohmann::json &json, Direction &direction);

/**
 * Read a Direction from bot serial format.
 * @param istream The input stream.
 * @param[out] direction The direction to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, Direction &direction);

class Map;

/** A location in Halatian space. */
struct Location {
    hlt::dimension_type pos_x; /**< Cartesian x-coordinate. */
    hlt::dimension_type pos_y; /**< Cartesian y-coordinate. */

    /**
     * Manhattan distance to another Location.
     * @param other The other Location.
     * @param width The width of the map/plane distance is being measured on (for wrap around computation)
     * @param height The height of the map/plane distance is being measured on (for wrap around computation)
     */

    hlt::dimension_type distance(const Location &other) const;

    /**
     * Move this Location in one direction.
     * @param direction The Direction in which to move.
     * @param map The Map on which to move.
     */
    void move_toward(const Direction &direction, const Map &map);

    /**
     * Convert a Location to JSON format.
     * @param[out] json The output JSON.
     * @param location The Location to convert.
     */
    friend void to_json(nlohmann::json &json, const Location &location);

    /**
     * Convert an encoded Location from JSON format.
     * @param json The JSON.
     * @param[out] location The converted Location.
     */
    friend void from_json(const nlohmann::json &json, Location &location);

    /** Test two Location instances for equality. */
    bool operator==(const Location &other) const {
        return pos_x == other.pos_x && pos_y == other.pos_y;
    }

    /**
     * Build a Location using Cartesian coordinates.
     * @param pos_x The x-coordinate.
     * @param pos_y the y-coordinate.
     */
    Location(hlt::dimension_type pos_x, hlt::dimension_type pos_y) : pos_x(pos_x), pos_y(pos_y) {}

    /** Default constructor is required by JSON deserialization. */
    Location() = default;
};

}

#endif // LOCATION_HPP
