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

    /** Run the game. */
    void run_game();

    /**
     * Determine whether the game has ended.
     *
     * @return True if the game has ended.
     */
    bool game_ended() const;

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

    /** Retrieve and process commands, and update the game state for the current turn. */
    void process_turn();

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
