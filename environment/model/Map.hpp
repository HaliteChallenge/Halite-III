#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <utility>
#include <vector>

#include "Cell.hpp"
#include "Entity.hpp"

namespace hlt {

namespace mapgen {
class Generator;
}

/** The map of cells. */
class Map {
    friend class hlt::mapgen::Generator;

    /** The type of the map grid. */
    using Grid = std::vector<std::vector<Cell>>;

public:
    /** The type of map dimension values. */
    using dimension_type = long;

    dimension_type width;         /**< The width of the map. */
    dimension_type height;        /**< The height of the map. */
    EntityFactory entity_factory; /**< The factory producing entities on this map. */
    Grid grid;                    /**< The map grid. */

    /**
     * Convert a Map to JSON format.
     * @param[out] json The output JSON.
     * @param map The Map to convert.
     */
    friend void to_json(nlohmann::json &json, const Map &map);

    /**
     * Convert an encoded Map from JSON format.
     * @param json The JSON.
     * @param[out] map The converted Map.
     */
    friend void from_json(const nlohmann::json &json, Map &map);

    /**
     * Write a Map to bot serial format.
     * @param ostream The output stream.
     * @param map The Map to write.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const Map &map);

private:
    /**
     * Create a Map from dimensions.
     * @param width The width.
     * @param height The height.
     */
    Map(dimension_type width, dimension_type height);

    /**
     * Create a Map from dimensions and grid.
     * @param width The width.
     * @param height The height.
     * @param grid The grid. Must be of correct dimensions.
     */
    Map(dimension_type width, dimension_type height, Map::Grid grid) :
            width(width), height(height), grid(std::move(grid)) {}
};

}

#endif /* MAP_H */
