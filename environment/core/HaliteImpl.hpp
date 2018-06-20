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
    //void process_production();

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
     *
     * @param entity_location
     * @param ownership_grid
     */
    void process_collision(Location entity_location, std::vector<std::vector<GridOwner>> &ownership_grid);

    void determine_cell_ownership(Location cell_location, std::vector<std::vector<GridOwner>> &ownership_grid);
    /**
     * Determine if "cell_to_check"'s closest entity is the same as that of "original_ownership_info"
     *
     * @param cell_to_check: Cell to determine ownership of
     * @param original_ownership_info: Ownership information of cell we reached cell_to_check from. This ownership
     * info is candidate ownership info for cell to check
     * @param ownership_grid Map sized grid storing cell ownership information as it is calculated
     * @return Boolean where true means that the owner of the original cell should also be the owner of cell to check
     */
    bool is_closest(Location cell_to_check, GridOwner original_ownership_info,
                    const std::vector<std::vector<GridOwner>> &ownership_grid);
    /**
     * Determine if
     * @param neighbor_location
     * @param current_cell_location
     * @param current_cell
     * @param ownership_grid
     */
    void check_update_tie(Location neighbor_location, Location current_cell_location, GridOwner current_cell,
                          std::vector<std::vector<GridOwner>> &ownership_grid);
    /**
     *
     * @param ownership_grid
     */
    void update_production_from_ownership(std::vector<std::vector<GridOwner>> &ownership_grid);
    /**
     *
     * @param ownership_grid
     * @param search_cells
     */
    void initialize_owner_search_from_sprites(std::vector<std::vector<GridOwner>> &ownership_grid, std::queue<Location> &search_cells);
    /**
     *
     * @param ownership_grid
     * @param search_cells
     */
    void run_initialized_owner_search(std::vector<std::vector<GridOwner>> &ownership_grid, std::queue<Location> &search_cells);

 public:
    /**
     * Construct HaliteImpl from game interface.
     * @param game The game interface.
     */
    explicit HaliteImpl(Halite *game) : game(game) {}

    void process_production();
};

}

#endif // HALITEIMPL_HPP
