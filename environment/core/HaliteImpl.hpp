#ifndef HALITEIMPL_HPP
#define HALITEIMPL_HPP

#include "Halite.hpp"

namespace hlt {

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
     * Determine entities closest to a given location
     * @param cell_location: Location of cell of which we want to find close entities
     * @param[out] close entities Filled during function with entities calculated to be at the same closest distance to relevant cell
     */
    void find_closest_entities(Location cell_location, std::vector<std::shared_ptr<Entity>> &close_entities);

    /**
     * Process production granting for a cell with tied ownership (ie multiple equidistant sprites of different players).
     * @param cell_location: Location object with details of cell with equidistant sprites
     * @param close entities Entities calculated to be at the same closest distance to relevant cell
     * @param turn_player_production Mapping from player id to production energy they gain during the turn.
     *     Will be updated to grant production of current cell to player that wins the tie
     */
    void process_tie(Location cell_location, std::vector<std::shared_ptr<Entity>> &close_entities,
                     std::unordered_map<id_type, energy_type> &turn_player_production);

public:
    /**
     * Construct HaliteImpl from game interface.
     * @param game The game interface.
     */
    explicit HaliteImpl(Halite *game) : game(game) {}
};

}

#endif // HALITEIMPL_HPP
