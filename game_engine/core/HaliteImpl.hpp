#ifndef HALITEIMPL_HPP
#define HALITEIMPL_HPP

#include <queue>

#include "CommandTransaction.hpp"
#include "Halite.hpp"
#include "Replay.hpp"
#include "Snapshot.hpp"
#include "BotError.hpp"

namespace hlt {

/** Halite implementation class, expressing core game logic. */
class HaliteImpl final {
    friend class Halite;

    /** The game interface. */
    Halite &game;

    /**
     * Initialize the game.
     * @param player_commands The list of player commands.
     */
    void initialize_game(const std::vector<std::string> &player_commands,
                         const Snapshot &snapshot);

    /** Run the game. */
    void run_game();

    /**
     * Determine whether the game has ended.
     *
     * @return True if the game has ended.
     */
    bool game_ended() const;

    /**
     * Determine whether a player can still play in the future
     *
     * @param player Player to check
     * @return True if the player can play on the next turn
     */
    bool player_can_play(const Player &player) const;

    /**
     * Update a player's statistics after a single turn. This will update their current game production and their last turn
     * alive if they are still alive.
     *
     * @param productions Mapping from player ID to the production they gained in the current turn.
     */
    void update_player_stats();

    /**
     * Determine if entity owned by given player is in range of another player (their entity, dropoff, or factory) and thus may interact
     *
     * param owner_id Id of owner of entity at given location
     * param entity_location Location of entity we are assessing for an interaction opportunity
     * return bool Indicator of whether there players are in close range for an interaction (true) or not (false)
     */
    bool possible_interaction(const Player::id_type owner_id, const Location entity_location);

    /**
     * Update players' rankings based on their final turn alive, then break ties with production totals in final turn.
     * Function is intended to be called at end of game, and will in place modify the ranking field of player statistics
     * to rank players from winner (1) to last player.
     */
    void rank_players();

    /** Update the inspiration flag on entities based on the current game state. */
    void update_inspiration();

    /** Retrieve and process commands, and update the game state for the current turn. */
    void process_turn();

    /** Remove a player from the game. */
    void kill_player(const Player::id_type &player_id);

    /**
     * Handle a player command error.
     * @param offenders The set of players this turn who have caused errors.
     * @param commands The player command mapping.
     * @param error The error caused by the player.
     */
    void handle_error(std::unordered_set<Player::id_type> &offenders,
                      ordered_id_map<Player, std::vector<std::unique_ptr<Command>>> &commands,
                      CommandError error);

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
