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

}
