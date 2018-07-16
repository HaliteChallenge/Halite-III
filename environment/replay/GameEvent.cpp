#include "GameEvent.hpp"

<<<<<<< HEAD
#include "nlohmann/json.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, x}

=======
>>>>>>> origin/shipyard
namespace hlt {

/** The JSON key for game event type. */
constexpr auto JSON_TYPE_KEY = "type";

/**
 * Convert a GameEvent to JSON format.
 * @param[out] json The output JSON.
 * @param cell The gameEvent to convert.
 */
void to_json(nlohmann::json &json, const GameEvent &game_event) { game_event->to_json(json); }

<<<<<<< HEAD
=======

>>>>>>> origin/shipyard
/**
 * Convert spawn event to json format
 * @param[out] json JSON to be filled by spawn event
 */
void SpawnEvent::to_json(nlohmann::json &json) const {
<<<<<<< HEAD
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
=======
    json = {{JSON_TYPE_KEY,     GAME_EVENT_TYPE_NAME},
            {JSON_LOCATION_KEY, location},
            {JSON_OWNER_KEY,    owner_id},
            {"energy",          energy}};
>>>>>>> origin/shipyard
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
