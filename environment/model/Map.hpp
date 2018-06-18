#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <utility>
#include <vector>

#include "Cell.hpp"
#include "Entity.hpp"
#include "Constants.hpp"

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
    hlt::dimension_type width;         /**< The width of the map. */
    hlt::dimension_type height;        /**< The height of the map. */
    Grid grid;                    /**< The map grid. */

    /**
     * Calculate the Manhattan distance between two cells on a grid
     *
     * @param x_cell1 The x location of the first cell
     * @param y_cell1 The y location of the first cell
     * @param x_cell2 The x location of the second cell
     * @param y_cell2 The y location of the second cell
     * @return The manhattan distance between the cells, calculated on a wrap around map
     */
    dimension_type distance(dimension_type x_cell1, dimension_type y_cell1, dimension_type x_cell2, dimension_type y_cell2);

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
    Map(hlt::dimension_type width, hlt::dimension_type height);

    /**
     * Create a Map from dimensions and grid.
     * @param width The width.
     * @param height The height.
     * @param grid The grid. Must be of correct dimensions.
     */
    Map(hlt::dimension_type width, hlt::dimension_type height, Map::Grid grid) :
            width(width), height(height), grid(std::move(grid)) {}
};

}

#endif /* MAP_H */
