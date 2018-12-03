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
    unsigned int random_id;                      /**< Random number assigned to player for use in ties. */
    long rank{};                                 /**< The rank of the player (1 = highest) */
    long last_turn_alive{};                      /**< The last turn the player remained alive */
    long last_turn_ship_spawn{};                 /**< The last turn the player spawned a ship */
    std::vector<energy_type> turn_productions{}; /**< Production granted to player each turn, turn 1 at front of vector. */
    std::vector<energy_type> turn_deposited{};   /**< Running total of how much halite has been collected by this player each turn. */
    energy_type total_production{};              /**< Production granted to player each turn, turn 1 at front of vector. */
    energy_type total_mined{};                   /**< Total amount extracted from map, including energy not deposited (lost due to collision or some other means), but not including bonuses from inspiration. */
    energy_type total_bonus{};                   /**< Total halite collected from inspiration bonuses, including energy not deposited (lost due to collision or some other means). */
    energy_type total_mined_from_captured{};     /**< Total amount mined, including energy not deposited (lost due to collision or some other means) and bonuses from inspiration, by ships captured from other players. */
    energy_type total_dropped{};                 /**< Total amount of halite lost due collisions. */
    energy_type carried_at_end{};                /**< Amount of halite on ships last frame. */
    dimension_type max_entity_distance{};        /**< The maximum distance any entity traveled from the player's factory. */
    dimension_type total_distance{};             /**< The total distance of all entities over all turns from factory. */
    dimension_type total_entity_lifespan{};      /**< Total lifespan of all entities (ie 1 entity for 10 turns plus 1 for 300 = 310. */
    long number_dropoffs{};                      /**< Number of dropoffs the player owns. */
    long interaction_opportunities{};            /**< The number of times a player's entity was within a short distance of another player. */
    long ships_captured{};                       /**< The number of ships captured by this player. */
    long ships_given{};                          /**< The number of ships captured from this player. */
    long self_collisions{};                      /**< The number of ships involved in collisions with allied ships. */
    long all_collisions{};                       /**< The number of ships involved in collisions with any ships, allied or not. Note there may be overlap with self_collisions if a 3+ ship collision occurs. */
    long dropoff_collisions{};                   /**< The number of ships involved in collisions with allied ships over a friendly dropoff. */
    long ships_spawned{};                        /**< The number of ships spawned. */
    long ships_peak{};                           /**< The maximum number of ship spawned at the same time. */
    std::unordered_map<Location, energy_type> halite_per_dropoff{}; /**< The amount of halite collected at each dropoff. */

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
     * @param random_id The random number assigned to this player to break ties in ranking players
     */
    explicit PlayerStatistics(Player::id_type player_id, unsigned int random_id) :
            player_id(player_id), random_id(random_id) {}
};

/** Statistics for a game. */
struct GameStatistics {
    std::vector<PlayerStatistics> player_statistics;        /**< Statistics for each player. */
    unsigned long number_turns{};                           /**< Total number of turns that finished before game ends. */
    energy_type map_total_halite{};                         /**< Total halite available at the start. */
    long long execution_time{};                             /**< Execution time of the game in ms. */

    unsigned long turn_number{};                            /**< Used to track last_turn_ship_spawn */

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
