#ifndef HALITEIMPL_HPP
#define HALITEIMPL_HPP

#include <queue>

#include "Halite.hpp"

namespace hlt {

constexpr id_type UNOWNED = -1;
constexpr id_type TIED = -2;

struct GridOwner {
    id_type owner = UNOWNED;
    std::vector<std::shared_ptr<Entity>> entities;
    dimension_type distance;
};
/** Halite implementation class, expressing core game logic. */
class HaliteImpl final {
    friend class Halite;

    /** Pointer to the game interface. Raw because HaliteImpl is always owned by Halite. */
    Halite *game;

    /**
     * Communicate with bots to obtain commands for next step.
     * @return Commands received from players.
     */
    std::unordered_map<id_type, Command> retrieve_commands();

    /**
     * Process the effects of commands.
     * @param commands The commands to process.
     */
    void process_commands(const std::unordered_map<id_type, Command> &commands);

    /** Process all entity lifecycle events for this turn. */
    void process_entities();

    /**
     * Process a spawn operation at a location, possibly merging onto an existing entity.
     * @param player The player owning the new entity.
     * @param location The location at which to spawn.
     */
    void spawn_entity(Player &player, const Location& location);

    /** Update the energy/production of each player by calculating ownership of each cell in the current turn */
    void process_production();

    /**
     * Process production granting for a cell with tied ownership (ie multiple equidistant sprites of different players).
     * @param cell_location: Location object with details of cell with equidistant sprites
     * @param close entities Entities calculated to be at the same closest distance to relevant cell
     * @param turn_player_production Mapping from player id to production energy they gain during the turn.
     *     Will be updated to grant production of current cell to player that wins the tie
     */
    void process_tie(Location cell_location, std::vector<std::shared_ptr<Entity>> &close_entities,
                     std::unordered_map<id_type, energy_type> &turn_player_production);
    /**
     * Given a location of a cell, return its 4 neighbors
     * @param location: the location of the cell we want the neighbors of
     * @return Vector of locations of length 4. A neighbor is a location with manhattan distance 1 from the first location.
     *  This function encapsulates the wrap around map - ie cell 0,0's neighbor's include cells at the very bottom and very right
     */
    std::vector<Location> get_neighbors(Location location);

    /**
     * Checks a cell to determine if multiple entities are on the cell
     * @param location. The location of the cell
     * @return Bool indicating if greater than 1 entity is on the cell.
     */
    bool multiple_entities_on_cell(Location location);

    /**
     * Determines the proper owner of a cell (or that the cell has tied ownership. Assumes cell_location neighbors at least
     * one owned cell
     *
     * @param cell_location Location of cell to determine ownership
     * @param ownership_grid 2D Grid storing details of cell ownership.
     */
    void determine_cell_ownership(Location cell_location, std::vector<std::vector<GridOwner>> &ownership_grid);

    /**
     * Grant production from owned cells on this turn to players
     *
     * Calculates total production gained on this turn for each player before adding to players' totals.
     * @param ownership_grid: 2d grid with each cell having an owner.
     */
    void update_production_from_ownership(std::vector<std::vector<GridOwner>> &ownership_grid);

    /**
     * Initialized ownership grid with locations of sprites. Assigns each cell to be owned by the sprite on it or determines
     * that there is a tie if two sprites share a cell. Initializes search_cells queue with cells with sprites on them.
     *
     * @param ownership_grid 2D grid in shape of game map storing details of cell ownership in a specific turn. Will update
     * ownership details as the search determines them.
     * @param search_cells Queue of cells that have been assigned an owner. After a cell's owner has been determined, it is added
     * to search_cells for the purpose of searching its neighbors.
     */
    void initialize_owner_search_from_sprites(std::vector<std::vector<GridOwner>> &ownership_grid, std::queue<Location> &search_cells);
    /**
     * Run modified BFS algorithm to determine owner (or tie) fof each cell. Assumes that search cells has been initized
     * with locations with sprites on them.
     *
     * @param ownership_grid 2D grid in shape of game map storing details of cell ownership in a specific turn. Will update
     * ownership details as the search determines them.
     * @param search_cells Queue of cells that have been assigned an owner. After a cell's owner has been determined, it is added
     * to search cells to search the neighbors of said cells.
     */
    void run_initialized_owner_search(std::vector<std::vector<GridOwner>> &ownership_grid, std::queue<Location> &search_cells);

 public:
    /**
     * Construct HaliteImpl from game interface.
     * @param game The game interface.
     */
    explicit HaliteImpl(Halite *game) : game(game) {}
};

}

#endif // HALITEIMPL_HPP
