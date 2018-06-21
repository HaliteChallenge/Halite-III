#ifndef GRID_HPP
#define GRID_HPP

#include "Location.hpp"

namespace hlt {

/** Template for classes representing grids indexable along two dimensions. */
template<class Entry>
class Grid {
protected:
    /** The type of the grid. */
    using grid_type = std::vector<std::vector<Entry>>;

    /** The internal data storage. */
    grid_type grid;

    /**
     * Create a Grid from dimensions.
     * @param width The width.
     * @param height The height.
     */
    Grid(dimension_type width, dimension_type height) : width(width), height(height) {
        grid.resize(static_cast<typename grid_type::size_type>(height));
        for (auto &row : grid) {
            row.resize(static_cast<typename std::remove_reference_t<decltype(row)>::size_type>(width));
        }
    }

    /** Default constructor. */
    Grid() = default;

public:
    dimension_type width{};   /**< The width of the grid. */
    dimension_type height{};  /**< The height of the grid. */

    /**
     * Convert a Grid to JSON format.
     * @tparam Entry The type of the grid entries.
     * @param[out] json The output JSON.
     * @param grid The Grid to convert.
     */
    friend void to_json(nlohmann::json &json, const hlt::Grid<Entry> &grid);

    /**
     * Get a reference to an entry at grid coordinates.
     * @param x The grid x-coordinate.
     * @param y The grid y-coordinate.
     * @return Reference to the entry at (x, y).
     */
    Entry &at(dimension_type x, dimension_type y) {
        // Flipping x and y gives us a grid memory representation that is consistent
        // with the physical grid, indexed by rows then columns.
        return grid[y][x];
    }

    /**
     * Get a const reference to an entry at grid coordinates.
     * @param x The grid x-coordinate.
     * @param y The grid y-coordinate.
     * @return Reference to the entry at (x, y).
     */
    const Entry &at(dimension_type x, dimension_type y) const {
        return grid[y][x];
    }

    /**
     * Get a reference to an entry at a location.
     * @param location The location.
     * @return Reference to the entry at (x, y).
     */
    Entry &at(const Location &location) {
        return grid[location.second][location.first];
    }

    /**
     * Get a const reference to an entry at a location.
     * @param location The location.
     * @return Reference to the entry at (x, y).
     */
    const Entry &at(const Location &location) const {
        return grid[location.second][location.first];
    }

    /**
     * Get a reference to an entry at a location.
     * @param location The location.
     * @return Reference to the entry at (x, y).
     */
    Entry &operator[](const Location &location) {
        return at(location);
    }

    /**
     * Get a const reference to an entry at a location.
     * @param location The location.
     * @return Reference to the entry at (x, y).
     */
    const Entry &operator[](const Location &location) const {
        return at(location);
    }
};

}

#endif // GRID_HPP
