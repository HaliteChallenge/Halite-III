#include "HaliteImpl.hpp"

namespace hlt {

/**
 * Update a player's statistics after a single turn. This will update their total game production, their last turn
 * alive if they are still alive, and the production for that turn.
 *
 * @param productions Mapping from player ID to the production they gained in the current turn.
 */
void HaliteImpl::update_player_stats(std::unordered_map<Player::id_type, energy_type> &productions) {
    for (PlayerStatistics &player_stats : game.game_statistics.player_statistics) {
        // Player with sprites is still alive, so mark as alive on this turn and add production gained
        if (!game.get_player(player_stats.player_id).entities.empty()) {
            player_stats.last_turn_alive = game.turn_number;
            player_stats.turn_productions.push_back(productions[player_stats.player_id]);
            player_stats.total_production += productions[player_stats.player_id];
        } else {
            player_stats.turn_productions.push_back(0);
        }
    }
}

/**
 * Update players' rankings based on their final turn alive, then break ties with production totals in final turn.
 * Function is intended to be called at end of game, and will in place modify the ranking field of player statistics
 * to rank players from winner (1) to last player.
 */
void HaliteImpl::rank_players() {
    auto &statistics = game.game_statistics.player_statistics;
    std::stable_sort(statistics.begin(), statistics.end());
    // Reverse list to have best players first
    std::reverse(statistics.begin(), statistics.end());

    for (size_t index = 0; index < statistics.size(); ++index) {
        statistics[index].rank = index + 1l;
    }
}

}
