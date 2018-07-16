#include "Statistics.hpp"

#include "nlohmann/json.hpp"

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
                return true;
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
    json = {FIELD_TO_JSON(player_id),
            FIELD_TO_JSON(rank),
            FIELD_TO_JSON(last_turn_alive),
            FIELD_TO_JSON(total_production)};
}

/**
 * Convert Game statistics to JSON format.
 * @param[out] json The output JSON.
 * @param stats The statistics to convert.
 */
void to_json(nlohmann::json &json, const GameStatistics &stats) {
    json = {FIELD_TO_JSON(number_turns),
            FIELD_TO_JSON(player_statistics)};
}

}
