#ifndef HALITE_STATISTICS_HPP
#define HALITE_STATISTICS_HPP

#include <set>
#include <string>
#include <vector>

#include "json.hpp"
#include "Constants.hpp"

namespace hlt {
class PlayerStatistics {
public:
    id_type player_id;                           /**< The ID of the player. */
    long rank{};                                 /**< The rank of the player (1 = highest) */
    unsigned long last_turn_alive{};             /**< The last turn the player remained alive */
    energy_type total_production{};              /**< Over the turns they were alive, the total production gained by a player */
    std::vector<energy_type> turn_productions{}; /**< Production granted to player each turn, turn 1 at front of vector */
    /**
     * Over the turns they were alive, the total number of cells a player owned. A cell counts towards this each turn
     * it is owned: ie if a player owns 0,0 for 10 turns, 0,0 contributes 10 to total_area
     */
    //dimension_type total_area{};

    friend void to_json(nlohmann::json &json, const PlayerStatistics &stats);

    /**
     * Construct PlayerStatistics from Player ID
     * @param player_id The player ID.
     */
    explicit PlayerStatistics(id_type player_id) : player_id(player_id) {}
};

struct GameStatistics {
    std::vector<PlayerStatistics> player_statistics;

    friend void to_json(nlohmann::json &json, const GameStatistics &stats);
};

}
#endif //HALITE_STATISTICS_HPP