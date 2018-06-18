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

    /**
     * Determine whether the game has ended.
     *
     * @return True if the game has ended.
     */
    bool game_ended() const;

public:
    /**
     * Construct HaliteImpl from game interface.
     * @param game The game interface.
     */
    explicit HaliteImpl(Halite *game) : game(game) {}
};

}

#endif // HALITEIMPL_HPP
