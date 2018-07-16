#include "GameEvent.hpp"

#include "nlohmann/json.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, x}

namespace hlt {

/** The JSON key for game event type. */
constexpr auto JSON_TYPE_KEY = "type";

/**
 * Convert a GameEvent to JSON format.
 * @param[out] json The output JSON.
 * @param cell The gameEvent to convert.
 */
void to_json(nlohmann::json &json, const GameEvent &game_event) { game_event->to_json(json);}

/**
 * Convert spawn event to json format
 * @param[out] json JSON to be filled by spawn event
 */
void SpawnEvent::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,      GAME_EVENT_TYPE_NAME},
            FIELD_TO_JSON(location),
            FIELD_TO_JSON(owner_id),
            FIELD_TO_JSON(id),
            FIELD_TO_JSON(energy)};
}

/**
 * Convert death event to json format
 * @param[out] json JSON to be filled with death event
 */
void CollisionEvent::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,     GAME_EVENT_TYPE_NAME},
            FIELD_TO_JSON(location),
            FIELD_TO_JSON(ships)};
}

/**
 * Convert death event to json format
 * @param[out] json JSON to be filled with death event
 */
void ConstructionEvent::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,     GAME_EVENT_TYPE_NAME},
            FIELD_TO_JSON(location),
            FIELD_TO_JSON(owner_id),
            FIELD_TO_JSON(id)};
}


}
