#include <set>

#include "Entity.hpp"
#include "Player.hpp"

#include "nlohmann/json.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, player.x}

/** Get a field from JSON. */
#define FIELD_FROM_JSON(x) json.at(#x)

namespace hlt {

/**
 * Convert a Player's entities to JSON format.
 * @param[out] json The output JSON.
 * @param entities The entities to convert.
 */
void to_json(nlohmann::json &json, const Player::Entities entities) {
    // Entities are a mapping from location to entity object
    // We want the json for an entity to include both the location and the details stored in the entity object
    json = nlohmann::json::array();
    for (const auto &[location, entity] : entities) {
        nlohmann::json entity_json;
        to_json(entity_json, entity);
        nlohmann::json location_json;
        to_json(location_json, location);
        entity_json.insert(location_json.begin(), location_json.end());
        json.push_back(entity_json);
    }
}

/**
 * Convert a Player to JSON format.
 * @param[out] json The output JSON.
 * @param player The Player to convert.
 */
void to_json(nlohmann::json &json, const Player &player) {
    json = {FIELD_TO_JSON(id),
            FIELD_TO_JSON(name),
            FIELD_TO_JSON(energy),
            FIELD_TO_JSON(factory),
            FIELD_TO_JSON(entities)};
}

/**
 * Write a Player to bot serial format.
 * @param ostream The output stream.
 * @param player The Player to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Player &player) {
    // Output player ID, number of entities, and current energy.
    ostream << player.id << " " << player.entities.size() << " " << player.energy << std::endl;
    // Output a list of entities.
    for (const auto &[location, entity] : player.entities) {
        auto [x, y] = location;
        ostream << x << " " << y << " " << entity;
    }
    return ostream;
}

}
