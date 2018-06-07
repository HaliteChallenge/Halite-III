#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "util/json.hpp"

namespace hlt {

/** The cardinal directions. */
enum class Direction {
    North = 0,
    South = 1,
    East = 2,
    West = 3,
};

class Map;

std::istream &operator>>(std::istream &is, Direction &direction);

/** A location in Halatian space. */
struct Location {
    long pos_x, pos_y; /**< Cartesian coordinates. */

    /** Manhattan distance between two locations. */
    long distance(const Location &other) const;

    /** Move this location in one direction. */
    void move_toward(const Direction &direction, const Map &map);

    friend void to_json(nlohmann::json &json, const Location &location);

    friend void from_json(const nlohmann::json &json, Location &location);

    /** Build a Location using coordinates. */
    Location(long pos_x, long pos_y);

    Location();
};

bool operator==(const Location &l1, const Location &l2);

}

#endif // LOCATION_HPP
