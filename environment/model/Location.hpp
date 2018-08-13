#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <functional>

#include "Constants.hpp"

#include "util.hpp"
#include "nlohmann/json_fwd.hpp"

namespace hlt {

/** The four cardinal directions. */
enum class Direction : char {
    North = 'n',
    South = 's',
    East = 'e',
    West = 'w',
    Still = 'o',
};

/**
 * Convert a Direction to JSON format.
 * @param[out] json The output JSON.
 * @param direction The Direction to convert.
 */
void to_json(nlohmann::json &json, const Direction &direction);

/**
 * Read a Direction from bot serial format.
 * @param istream The input stream.
 * @param[out] direction The direction to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, Direction &direction);

/** Type of a location. */
struct Location final {
    dimension_type x; /**< The x-coordinate. */
    dimension_type y; /**< The y-coordinate. */

    /**
     * Compare to another Location by equality.
     * @param location The other Location.
     * @return True if this Location is equal to the other.
     */
    bool operator==(const Location &location) const {
        return this->x == location.x && this->y == location.y;
    }

    /**
     * Write a Location to bot serial format.
     * @param ostream The output stream.
     * @param location The Location to write.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const Location &location);

    /**
     * Convert a Location to JSON format.
     * @param[out] json The output JSON.
     * @param location The Location to convert.
     */
    friend void to_json(nlohmann::json &json, const Location &location);

    /**
     * Construct Location from coordinates.
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     */
    Location(dimension_type x, dimension_type y) : x(x), y(y) {}
};

}

namespace std {
template<>
struct hash<hlt::Location> {
    size_t operator()(const hlt::Location &location) const {
        return hash_combine(static_cast<size_t>(location.x), static_cast<size_t>(location.y));
    }
};
}

#endif // LOCATION_HPP
