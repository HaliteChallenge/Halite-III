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
    friend void to_json(nlohmann::json &json, const PlayerStatistics &statistics);

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
    std::vector<PlayerStatistics> player_statistics;     /**< Vector of statistics for each player. */
    unsigned long number_turns{};                        /**< Total number of turns that finished before game ende. */

    /**
     * Convert game statistics to json format
     *
     * @param[out] json Empty json object to fill
     * @param statistics Game statistics struct to convert to json
     */
    friend void to_json(nlohmann::json &json, const GameStatistics &statistics);
};

}
#endif //HALITE_STATISTICS_HPP
