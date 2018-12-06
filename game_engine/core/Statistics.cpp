#include "Statistics.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, stats.x}

namespace hlt {

/**
 * Compare two players to rank them.
 *
 * @param other The statistics of the other player.
 * @return True if this player ranks below (i.e. is worse than) the other.
 */
bool PlayerStatistics::operator<(const PlayerStatistics &other) const {
    if (this->last_turn_alive == other.last_turn_alive) {
        auto turn_to_compare = this->last_turn_alive;
        while (this->turn_productions[turn_to_compare] == other.turn_productions[turn_to_compare]) {
            if (--turn_to_compare < 0) {
                // Players exactly tied on all turns, so randomly choose
                return this->random_id < other.random_id;
            }
        }
        return this->turn_productions[turn_to_compare] < other.turn_productions[turn_to_compare];
    } else {
        return this->last_turn_alive < other.last_turn_alive;
    }
}

/**
 * Convert Player statistics to JSON format.
 * @param[out] json The output JSON.
 * @param stats The statistics to convert.
 */
void to_json(nlohmann::json &json, const PlayerStatistics &stats) {
    dimension_type average_distance = 0;
    if (stats.total_entity_lifespan != 0) {
        average_distance = stats.total_distance / stats.total_entity_lifespan;
    }
    energy_type final_production = 0;
    if (!stats.turn_productions.empty()) {
        final_production = stats.turn_productions.back();
    }
    double mining_efficiency = 0.0;

    if (stats.total_mined > 0) {
        mining_efficiency = stats.total_production / static_cast<double>(stats.total_mined);
    }

    json = {FIELD_TO_JSON(player_id),
            FIELD_TO_JSON(random_id),
            FIELD_TO_JSON(rank),
            FIELD_TO_JSON(last_turn_alive),
            FIELD_TO_JSON(last_turn_ship_spawn),
            {"final_production", final_production},
            FIELD_TO_JSON(total_production),
            FIELD_TO_JSON(max_entity_distance),
            FIELD_TO_JSON(number_dropoffs),
            FIELD_TO_JSON(interaction_opportunities),
            //FIELD_TO_JSON(ships_captured),
            //FIELD_TO_JSON(ships_given),
            FIELD_TO_JSON(ships_spawned),
            FIELD_TO_JSON(ships_peak),
            FIELD_TO_JSON(self_collisions),
            FIELD_TO_JSON(all_collisions),
            FIELD_TO_JSON(dropoff_collisions),
            FIELD_TO_JSON(total_mined),
            FIELD_TO_JSON(total_bonus),
            //FIELD_TO_JSON(total_mined_from_captured),
            FIELD_TO_JSON(total_dropped),
            FIELD_TO_JSON(carried_at_end),
            {"mining_efficiency", mining_efficiency},
            FIELD_TO_JSON(halite_per_dropoff),
            {"average_entity_distance", average_distance}};
}

/**
 * Convert Game statistics to JSON format.
 * @param[out] json The output JSON.
 * @param stats The statistics to convert.
 */
void to_json(nlohmann::json &json, const GameStatistics &stats) {
    json = {FIELD_TO_JSON(number_turns),
            FIELD_TO_JSON(player_statistics),
            FIELD_TO_JSON(execution_time),
            FIELD_TO_JSON(map_total_halite)};
}

}
