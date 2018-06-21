#include "Entity.hpp"

#include "nlohmann/json.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, player.x}

/** Get a field from JSON. */
#define FIELD_FROM_JSON(x) json.at(#x)

namespace hlt {

/**
 * Convert a Player to JSON format.
 * @param[out] json The output JSON.
 * @param player The Player to convert.
 */
// TODO: add functionality to convert entity mapping to json
void to_json(nlohmann::json &json, const Player &player) {
    json = {FIELD_TO_JSON(player_id),
            FIELD_TO_JSON(name),
            FIELD_TO_JSON(energy),
            FIELD_TO_JSON(factory_location)};
}

/**
 * Write a Player to bot serial format.
 * @param ostream The output stream.
 * @param player The Player to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Player &player) {
    // Output player ID, number of entities, and current energy.
    ostream << player.player_id << " " << player.entities.size() << " " << player.energy << std::endl;
    // Output a list of entities.
    for (const auto &location_entity_pair : player.entities) {
        const std::pair<dimension_type, dimension_type> &location = location_entity_pair.first;
        ostream << location.first << " " << location.second << " " << *(location_entity_pair.second);
    }
    return ostream;
}

/**
 * Add an entity by location, possibly merging with an existing entity.
 * @param location The location for the entity.
 * @param entity The entity to add.
 */
void Player::add_entity(const Location &location, std::shared_ptr<Entity> &entity) {
    auto entity_iterator = entities.find(location);
    if (entity_iterator != entities.end()) {
        // If the player already has an entity there, merge
        entity_iterator->second->energy += entity->energy;
    } else {
        // Otherwise, move this entity there
        entities[location] = entity;
    }
}

/**
 * Remove an entity by location.
 * @param location The location of the entity.
 */
void Player::remove_entity(const Location &location) {
    entities.erase(location);
}

}
