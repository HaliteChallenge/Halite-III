#ifndef MAP_H
#define MAP_H

#include <array>
#include <iostream>
#include <utility>
#include <vector>

#include "Cell.hpp"
#include "Constants.hpp"
#include "Entity.hpp"
#include "Grid.hpp"
#include "Location.hpp"
#include "Transaction.hpp"

namespace hlt {

namespace mapgen {
class Generator;
}

class Map;

/** Transactions on Maps, which execute a series of player commands atomically. */
class MapTransaction : public Transaction {
    /** The command buffer. */
    std::vector<std::tuple<Player, Location, Location>> commands;

public:
    /** The map. */
    Map &map;

    /**
     * Construct MapTransaction from Map.
     * @param map The Map.
     */
    explicit MapTransaction(Map &map) : map(map) {}

    /**
     * Attempt to commit the transaction.
     * @return True if the commit succeeded.
     */
    bool commit() override;

    /**
     * Add an entity move to the transaction.
     *
     * @param player The player owning the entity.
     * @param from The source location of the entity.
     * @param to The destination location of the entity.
     */
    void move_entity(const Player &player, const Location &from, const Location &to) {
        commands.emplace_back(player, from, to);
    }
};

/** The map of cells. */
class Map : public Grid<Cell>, public Transactional<MapTransaction> {
    friend class hlt::mapgen::Generator;

public:
    std::string map_generator = "none";    /**< The name of the map generator used to generate the map */
    /** The number of neighbors of each cell on the map grid. */
    static constexpr auto NEIGHBOR_COUNT = 4;

    using Transaction = MapTransaction;

    /**
     * Convert a Map to JSON format.
     * @param[out] json The output JSON.
     * @param map The Map to convert.
     */
    friend void to_json(nlohmann::json &json, const Map &map);

    /**
     * Convert this map to JSON format.
     * Included in addition to above to gain access to grid member of super class
     *
     * @param[out] json The output JSON.
     */
    void to_json(nlohmann::json &json) const;

    /**
     * Given a location of a cell, return its neighbors.
     * @param location The location of the cell we want the neighbors of.
     * @return Array of neighbor locations.
     *  A neighbor is a location with Manhattan distance 1 from the input location.
     *  This function encapsulates the wrap-around map -
     *  i.e. cell (0, 0)'s neighbors include cells at the very bottom and very right of the map.
     */
    std::array<Location, NEIGHBOR_COUNT> get_neighbors(const Location &location) const;

    /**
     * Calculate the Manhattan distance between two cells on a grid.
     *
     * @param from The location of the first cell.
     * @param to The location of the second cell.
     * @return The Manhattan distance between the cells, calculated on a wrap-around map.
     */
    dimension_type distance(const Location &from, const Location &to) const;

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
    void move_location(Location &location, const Direction &direction) const;

    /**
     * Begin a transaction.
     *
     * @return New Transaction object.
     */
    Transaction begin_transaction() override { return Transaction(*this); };

private:
    /**
     * Create a Map from dimensions.
     * @param width The width.
     * @param height The height.
     */
    Map(dimension_type width, dimension_type height) : Grid(width, height) {}

    /**
     * Create a Map from dimensions and grid.
     * @param width The width.
     * @param height The height.
     * @param grid The grid. Must be of correct dimensions.
     */
    Map(dimension_type width, dimension_type height, grid_type grid) {
        this->width = width;
        this->height = height;
        this->grid = std::move(grid);
    }
};

}

#endif /* MAP_H */
