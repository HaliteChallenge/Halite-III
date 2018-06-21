#include "Map.hpp"

#include "nlohmann/json.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, map.x}

/** Get a field from JSON. */
#define FIELD_FROM_JSON(x) json.at(#x)

namespace hlt {

/**
 * Create a Map from dimensions.
 * @param width The width.
 * @param height The height.
 */
Map::Map(hlt::dimension_type width, hlt::dimension_type height) : width(width), height(height) {
    grid.resize(static_cast<decltype(grid)::size_type>(height));
    for (auto &row : grid) {
        row.resize(static_cast<std::remove_reference_t<decltype(row)>::size_type>(width));
    }
}

/**
 * Given a location of a cell, return its neighbors.
 * @param location The location of the cell we want the neighbors of.
 * @return Array of neighbor locations.
 *  A neighbor is a location with manhattan distance 1 from the input location.
 *  This function encapsulates the wrap-around map -
 *  i.e. cell (0, 0)'s neighbors include cells at the very bottom and very right of the map.
 */
std::array<Location, Map::NEIGHBOR_COUNT> Map::get_neighbors(const Location &location) const {
    // Allow wrap around neighbors
    auto x = location.first;
    auto y = location.second;
    return {{{(x + 1) % width, y}, {(x - 1 + width) % width, y}, {x, (y + 1) % height}, {x, (y - 1 + height) % height}}};
}

/**
 * Calculate the Manhattan distance between two cells on a grid.
 *
 * @param cell1 The location of the first cell.
 * @param cell2 The location of the second cell.
 * @return The Manhattan distance between the cells, calculated on a wrap-around map.
 */
dimension_type Map::distance(const Location &cell1, const Location &cell2) const {
    const auto x_dist = abs(cell1.first - cell2.first);
    const auto y_dist = abs(cell1.second - cell2.second);
    return std::min(x_dist, width - x_dist) + std::min(y_dist, height - y_dist);
}

/**
 * Convert a Map to JSON format.
 * @param[out] json The output JSON.
 * @param map The Map to convert.
 */
void to_json(nlohmann::json &json, const Map &map) {
    json = {FIELD_TO_JSON(width),
            FIELD_TO_JSON(height),
            FIELD_TO_JSON(grid)};
}

/**
 * Convert an encoded Map from JSON format.
 * @param json The JSON.
 * @param[out] map The converted Map.
 */
void from_json(const nlohmann::json &json, Map &map) {
    map = {FIELD_FROM_JSON(width),
           FIELD_FROM_JSON(height),
           FIELD_FROM_JSON(grid)};
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
            os << cell;
        }
    }
    return os;
}

/**
 * Move a location in a direction.
 * @param location The location to move.
 * @param direction The direction to move it in.
 */
void Map::move_location(Location &location, const Direction &direction) {
    switch (direction) {
    case Direction::North:
        location.second = (location.second + height - 1) % height;
        break;
    case Direction::South:
        location.second = (location.second + 1) % height;
        break;
    case Direction::East:
        location.first = (location.first + 1) % width;
        break;
    case Direction::West:
        location.first = (location.first + width - 1) % width;
        break;
    }
}

}
