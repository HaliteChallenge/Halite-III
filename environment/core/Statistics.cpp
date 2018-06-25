#include "Statistics.hpp"
#include "nlohmann/json.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, stats.x}

/** Get a field from JSON. */
#define FIELD_FROM_JSON(x) json.at(#x)

namespace hlt {

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