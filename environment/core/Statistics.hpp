#ifndef HALITE_STATISTICS_HPP
#define HALITE_STATISTICS_HPP

#include <vector>

#include "Constants.hpp"
#include "Player.hpp"

#include "nlohmann/json_fwd.hpp"

namespace hlt {

/** Statistics for a player in the game. */
struct PlayerStatistics {
    Player::id_type player_id;                   /**< The ID of the player. */
    long rank{};                                 /**< The rank of the player (1 = highest) */
    long last_turn_alive{};                      /**< The last turn the player remained alive */
    energy_type total_production{};              /**< Over the turns they were alive, the total production gained by a player */
    std::vector<energy_type> turn_productions{}; /**< Production granted to player each turn, turn 1 at front of vector */

    /**
     * Convert Player statistics to JSON format.
     * @param[out] json The output JSON.
     * @param stats The statistics to convert.
     */
    friend void to_json(nlohmann::json &json, const PlayerStatistics &stats);

    /**
     * Compare two players to rank them.
     *
     * @param other The statistics of the other player.
     * @return True if this player ranks below (i.e. is worse than) the other.
     */
    bool operator<(const PlayerStatistics &other) const;

    /**
     * Construct PlayerStatistics from Player ID.
     * @param player_id The player ID.
     */
    explicit PlayerStatistics(Player::id_type player_id) : player_id(player_id) {}
};

/** Statistics for a game. */
struct GameStatistics {
    /** The collection of player statistics. */
    std::vector<PlayerStatistics> player_statistics;

    /**
     * Convert Game statistics to JSON format.
     * @param[out] json The output JSON.
     * @param stats The statistics to convert.
     */
    friend void to_json(nlohmann::json &json, const GameStatistics &stats);
};

}
#endif //HALITE_STATISTICS_HPP
