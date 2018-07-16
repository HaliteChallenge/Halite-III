#include <set>

#include "Entity.hpp"
#include "Player.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, player.x}

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
    for (const auto &[id, location_entity] : entities) {
        const auto &[_, entity] = location_entity;
        nlohmann::json entity_json;
        to_json(entity_json, entity);
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
    for (const auto &[id, location_entity] : player.entities) {
        auto [location, entity] = location_entity;
        ostream << id << " " << location << " " << entity;
    }
    return ostream;
}

/**
 * Get the location of an entity.
 * @param id The entity ID.
 * @return The entity location.
 */
Location Player::get_entity_location(const Entity::id_type &id) const {
    return entities.find(id)->second.first;
}

/**
 * Get an entity by ID.
 * @param id The entity ID.
 * @return The entity.
 */
Entity &Player::get_entity(const Entity::id_type &id) {
    return entities.find(id)->second.second;
}

/**
 * Get whether the player has an entity.
 * @param id The entity ID.
 * @return True if the player has the entity, false otherwise.
 */
bool Player::has_entity(const Entity::id_type &id) const {
    return entities.find(id) == entities.end();
}

/**
 * Remove an entity by ID.
 * @param id The entity ID.
 */
void Player::remove_entity(const Entity::id_type &id) {
    entities.erase(id);
}

/**
 * Add an entity.
 * @param entity The entity to add.
 * @param location The location of the entity.
 */
void Player::add_entity(Entity &entity, Location location) {
    entities.emplace(entity.id, std::pair<Location, std::reference_wrapper<Entity>>(location, entity));
}

}
