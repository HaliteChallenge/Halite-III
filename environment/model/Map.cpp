#include <set>
#include <Logging.hpp>

#include "Map.hpp"

#include "nlohmann/json.hpp"

/** Convert a field to JSON. */
#define FIELD_TO_JSON(x) {#x, x}

/** Get a field from JSON. */
#define FIELD_FROM_JSON(x) json.at(#x)

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

/**
 * Calculate the Manhattan distance between two cells on a grid.
 *
 * @param from location of the first cell.
 * @param to The location of the second cell.
 * @return The Manhattan distance between the cells, calculated on a wrap-around map.
 */
void to_json(nlohmann::json &json, const Map &map) {
    map.to_json(json);
}

/**
 * Convert this map to JSON format.
 * Included in addition to above to gain access to grid member of super class
 *
 * @param[out] json The output JSON.
 */
void Map::to_json(nlohmann::json &json) const {
    json = {FIELD_TO_JSON(height),
            FIELD_TO_JSON(width),
            FIELD_TO_JSON(map_generator),
            FIELD_TO_JSON(grid)};
}

dimension_type Map::distance(const Location &from, const Location &to) const {
    const auto [from_x, from_y] = from;
    const auto [to_x, to_y] = to;
    const auto x_dist = std::abs(from_x - to_x);
    const auto y_dist = std::abs(from_y - to_y);
    return std::min(x_dist, width - x_dist) + std::min(y_dist, height - y_dist);
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
    }
}

/**
 * Attempt to commit the transaction.
 * @return True if the commit succeeded.
 */
bool MapTransaction::commit() {
    std::set<std::pair<Player, Location>> command_set;
    for (const auto &[player, from, to] : commands) {
        if (map.at(from)->find_entity(player) == nullptr) {
            return false;
        }
        if (const auto &[_, inserted] = command_set.emplace(player, from); !inserted) {
            // Duplicate found
            return false;
        }
    }
    std::unordered_map<Player, std::vector<std::pair<Location, std::shared_ptr<Entity>>>> moved_entities;
    for (const auto &[player, from, to] : commands) {
        moved_entities[player].emplace_back(to, map.at(from)->remove_entity(player));
    }
    for (auto &[player, entities] : moved_entities) {
        for (auto &[location, entity] : entities) {
            map.at(location)->add_entity(player, std::move(entity));
        }
    }
    return true;
}

}
