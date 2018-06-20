#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <utility>
#include <vector>

#include "Cell.hpp"
#include "Constants.hpp"
#include "Entity.hpp"
#include "Location.hpp"

namespace hlt {

namespace mapgen {
class Generator;
}

/** The map of cells. */
class Map {
    friend class hlt::mapgen::Generator;

    /** The type of the map grid. */
    using Grid = std::vector<std::vector<Cell>>;

    Grid grid;                         /**< The map grid. */

public:
    hlt::dimension_type width;         /**< The width of the map. */
    hlt::dimension_type height;        /**< The height of the map. */

    /**
     * Get a reference to a cell at grid coordinates.
     * @param x The grid x-coordinate.
     * @param y The grid y-coordinate.
     * @return Reference to the cell at (x, y).
     */
    Cell &at(dimension_type x, dimension_type y) {
        // Flipping x and y gives us a grid memory representation that is consistent
        // with the physical grid, indexed by rows then columns.
        return grid[y][x];
    }

    /**
     * Get a const reference to a cell at grid coordinates.
     * @param x The grid x-coordinate.
     * @param y The grid y-coordinate.
     * @return Reference to the cell at (x, y).
     */
    const Cell &at(dimension_type x, dimension_type y) const {
        return grid[y][x];
    }

    /**
     * Get a reference to a cell at a location.
     * @param location The location.
     * @return Reference to the cell at (x, y).
     */
    Cell &at(const Location &location) {
        return grid[location.second][location.first];
    }

    /**
     * Get a const reference to a cell at a location.
     * @param location The location.
     * @return Reference to the cell at (x, y).
     */
    const Cell &at(const Location &location) const {
        return grid[location.second][location.first];
    }

    /**
     * Calculate the Manhattan distance between two cells on a grid
     *
     * @param cell1 The location of the first cell
     * @param cell2 The location of the second cell
     * @return The manhattan distance between the cells, calculated on a wrap around map
     */
    dimension_type distance(Location cell1, Location cell2);

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

    /**
     * Move a location in a direction.
     * @param location The location to move.
     * @param direction The direction to move it in.
     */
    void move_location(Location &location, const Direction &direction);

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
            grid(std::move(grid)), width(width), height(height) {}
};

}

#endif /* MAP_H */
