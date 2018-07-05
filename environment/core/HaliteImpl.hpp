#ifndef HALITEIMPL_HPP
#define HALITEIMPL_HPP

#include <queue>

#include "Halite.hpp"
#include "Replay.hpp"
#include "OwnershipGrid.hpp"

namespace hlt {

/** Halite implementation class, expressing core game logic. */
class HaliteImpl final {
    friend class Halite;

    /** The game interface. */
    Halite &game;

    /** Representation of the map grid containing ownership information. */
    OwnershipGrid ownership_grid;

    /** Current commands per player. */
    std::unordered_map<Player::id_type, std::vector<Command>> commands;

    /**
     * Determine whether the game has ended.
     *
     * @return True if the game has ended.
     */
    bool game_ended() const;

    /** Communicate with bots to obtain commands for next step. */
    void retrieve_commands();

    /** Process the effects of retrieved commands. */
    void process_commands();

    /**
     * Process a spawn operation at a location, possibly merging onto an existing entity.
     * @param player The player owning the new entity.
     * @param location The location at which to spawn.
     */
    void spawn_entity(Player &player, const Location &location);

    /** Process all entity lifecycle events for this turn. */
    void process_entities();

    /**
     * Update a player's statistics after a single turn. This will update their total game production, their last turn
     * alive if they are still alive, and the production for that turn.
     *
     * @param productions Mapping from player ID to the production they gained in the current turn.
     */
    void update_player_stats(std::unordered_map<Player::id_type, energy_type> &productions);

    /**
     * Update players' rankings based on their final turn alive, then break ties with production totals in final turn.
     * Function is intended to be called at end of game, and will in place modify the ranking field of player statistics
     * to rank players from winner (1) to last player.
     */
    void rank_players();

    /** Update the energy/production of each player by calculating ownership of each cell in the current turn. */
    void process_production();

    /**
     * Initialized ownership grid with locations of sprites. Assigns each cell to be owned by the sprite on it or determines
     * that there is a tie if two sprites share a cell. Initializes search_cells queue with cells with sprites on them.
     *
     * @param search_cells Queue of cells that have been assigned an owner. After a cell's owner has been determined, it is added
     * to search_cells for the purpose of searching its neighbors.
     */
    void initialize_owner_search_from_sprites(std::queue<Location> &search_cells);

    /**
     * Run modified BFS algorithm to determine owner (or tie) of each cell. Assumes that search cells has been initialized
     * with locations with sprites on them.
     *
     * @param search_cells Queue of cells that have been assigned an owner. After a cell's owner has been determined, it is added
     * to search cells to search the neighbors of said cells.
     */
    void run_initialized_owner_search(std::queue<Location> &search_cells);

    /**
     * Grant production from owned cells on this turn to players.
     * Calculates total production gained on this turn for each player before adding to players' totals.
     */
    void update_production_from_ownership();

    /**
     * Determines the proper owner of a cell (or that the cell has tied ownership).
     * Assumes cell_location neighbors at least one owned cell.
     *
     * @param cell_location Location of cell to determine ownership.
     */
    void determine_cell_ownership(const Location &cell_location);

    /**
     * Determine whether multiple entities are on a cell.
     * @param location The location of the cell.
     * @return True if greater than 1 entity is on the cell.
     */
    bool multiple_entities_on_cell(const Location &location) const;

    /**
     * Process production granting for a cell with tied ownership
     * (i.e. multiple equidistant sprites of different players).
     * @param cell_location Location object with details of cell with equidistant sprites.
     * @param close_entities Entities calculated to be at the same closest distance to relevant cell.
     * @param turn_player_production Mapping from player ID to production energy they gain during the turn.
     *     Will be updated to grant production of current cell to player that wins the tie.
     */
    void process_tie(const Location &cell_location, std::unordered_set<std::shared_ptr<PlayerEntity>> &close_entities,
                     std::unordered_map<Player::id_type, energy_type> &turn_player_production);

    /**
     * Process the search initialization for a cell with colliding entities
     * (i.e. entities from multiple different players on one cell).
     *
     * @param cell_location Location of cell with colliding entities.
     * @return Resulting owner for this cell.
     */
    Player::id_type process_collision(const Location &cell_location);

public:
    /**
     * Construct HaliteImpl from game interface.
     *
     * @param game The game interface.
     */
    explicit HaliteImpl(Halite &game);
};

}

#endif // HALITEIMPL_HPP
