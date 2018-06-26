#include "GameEvent.hpp"

#include "nlohmann/json.hpp"

namespace hlt {

/** The JSON key for game event type. */
constexpr auto JSON_TYPE_KEY = "type";
constexpr auto JSON_LOCATION_KEY = "location";
constexpr auto JSON_OWNER_KEY = "owner_id";

/**
 * Convert a GameEvent to JSON format.
 * @param[out] json The output JSON.
 * @param cell The gameEvent to convert.
 */
void to_json(nlohmann::json &json, const GameEvent &game_event) { game_event->to_json(json);}


constexpr char const *SpawnEvent::GAME_EVENT_TYPE_NAME;
/**
 * Convert spawn event to json format
 * @param[out] json JSON to be filled by spawn event
 */
void SpawnEvent::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,      GAME_EVENT_TYPE_NAME},
            {JSON_LOCATION_KEY,  location},
            {JSON_OWNER_KEY,     owner_id},
            {"energy",           energy}};
}

constexpr char const *DeathEvent::GAME_EVENT_TYPE_NAME;
/**
 * Convert death event to json format
 * @param[out] json JSON to be filled with death event
 */
void DeathEvent::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,     GAME_EVENT_TYPE_NAME},
            {JSON_LOCATION_KEY, location},
            {JSON_OWNER_KEY,    owner_id}};
}

}
