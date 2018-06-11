#include "Map.hpp"

/** The JSON key for width. */
constexpr auto JSON_WIDTH = "width";
/** The JSON key for height. */
constexpr auto JSON_HEIGHT = "height";
/** The JSON key for grid. */
constexpr auto JSON_GRID = "grid";

namespace hlt {

/**
 * Create a Map from dimensions.
 * @param width The width.
 * @param height The height.
 */
Map::Map(dimension_type width, dimension_type height) : width(width), height(height) {
    grid.resize(static_cast<decltype(grid)::size_type>(height));
    for (auto &row : grid) {
        row.resize(static_cast<std::remove_reference_t<decltype(row)>::size_type>(width));
    }
}

/**
 * Convert a Map to JSON format.
 * @param[out] json The output JSON.
 * @param map The Map to convert.
 */
void to_json(nlohmann::json &json, const Map &map) {
    json = {{JSON_WIDTH,  map.width},
            {JSON_HEIGHT, map.height},
            {JSON_GRID,   map.grid}};
}

/**
 * Convert an encoded Map from JSON format.
 * @param json The JSON.
 * @param[out] map The converted Map.
 */
void from_json(const nlohmann::json &json, Map &map) {
    map = {json.at(JSON_WIDTH).get<decltype(map.width)>(),
           json.at(JSON_HEIGHT).get<decltype(map.height)>(),
           json.at(JSON_GRID).get<decltype(map.grid)>()};
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

}
