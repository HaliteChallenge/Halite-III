#ifndef HALITEIMPL_HPP
#define HALITEIMPL_HPP

#include <queue>

#include "Halite.hpp"
#include "Replay.hpp"

namespace hlt {

/** Halite implementation class, expressing core game logic. */
class HaliteImpl final {
    friend class Halite;

    /** The game interface. */
    Halite &game;

    /** Current commands per player. */
    id_map<Player, std::vector<std::unique_ptr<Command>>> commands;

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

    /**
     * Kill a player, for example if they performed an illegal behavior.
     * @param id The ID of the player to kill.
     */
    void kill_player(Player::id_type id);

    /** Update the energy/production of each player by calculating ownership of each cell in the current turn. */
    void process_production();

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
