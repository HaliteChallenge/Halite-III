#include "Map.hpp"

/** Convert a field to JSON. */
#define FIELD_TO_JSON(x) {#x, map.x}

namespace hlt {

/**
 * Given a location of a cell, return its neighbors.
 * @param location The location of the cell we want the neighbors of.
 * @return Array of neighbor locations.
 *  A neighbor is a location with Manhattan distance 1 from the input location.
 *  This function encapsulates the wrap-around map -
 *  i.e. cell (0, 0)'s neighbors include cells at the very bottom and very right of the map.
 */
std::array<Location, Map::NEIGHBOR_COUNT> Map::get_neighbors(const Location &location) const {
    // Allow wrap around neighbors
    auto [x, y] = location;
    return {{{(x + 1) % width, y},
             {(x - 1 + width) % width, y},
             {x, (y + 1) % height},
             {x, (y - 1 + height) % height}}};
}

void to_json(nlohmann::json &json, const Map &map) {
    json = {FIELD_TO_JSON(height),
            FIELD_TO_JSON(width),
            FIELD_TO_JSON(grid)};
}

/**
 * Calculate the Manhattan distance between two cells on a grid.
 *
 * @param from location of the first cell.
 * @param to The location of the second cell.
 * @return The Manhattan distance between the cells, calculated on a wrap-around map.
 */
dimension_type Map::distance(const Location &from, const Location &to) const {
    const auto [from_x, from_y] = from;
    const auto [to_x, to_y] = to;
    const auto x_dist = std::abs(from_x - to_x);
    const auto y_dist = std::abs(from_y - to_y);
    return std::min(x_dist, width - x_dist) + std::min(y_dist, height - y_dist);
}

/**
 * Write a Map to bot serial format.
 * @param ostream The output stream.
 * @param map The Map to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &os, const Map &map) {
    // Output the map dimensions.
    os << map.width << " " << map.height << std::endl;
    // Output the cells one after another.
    for (const auto &row : map.grid) {
        for (const auto &cell : row) {
            os << cell << " ";
        }
        os << std::endl;
    }
    return os;
}

/**
 * Move a location in a direction.
 * @param location The location to move.
 * @param direction The direction to move it in.
 */
void Map::move_location(Location &location, const Direction &direction) const {
    auto &[x, y] = location;
    switch (direction) {
    case Direction::North:
        y = (y + height - 1) % height;
        break;
    case Direction::South:
        y = (y + 1) % height;
        break;
    case Direction::East:
        x = (x + 1) % width;
        break;
    case Direction::West:
        x = (x + width - 1) % width;
        break;
    case Direction::Still:
        // Don't move
        break;
    }
}

}
