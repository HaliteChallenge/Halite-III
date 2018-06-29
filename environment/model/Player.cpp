#include <set>

#include "Entity.hpp"

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
        to_json(entity_json, *entity, false);
        entity_json["x"] = location.first;
        entity_json["y"] = location.second;
        json.push_back(entity_json);
    }
}

/**
 * Convert a Player to JSON format.
 * @param[out] json The output JSON.
 * @param player The Player to convert.
 */
void to_json(nlohmann::json &json, const Player &player) {
    json = {FIELD_TO_JSON(player_id),
            FIELD_TO_JSON(name),
            FIELD_TO_JSON(energy),
            FIELD_TO_JSON(factory_location),
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
    ostream << player.player_id << " " << player.entities.size() << " " << player.energy << std::endl;
    // Output a list of entities.
    for (const auto &[location, entity] : player.entities) {
        auto [x, y] = location;
        ostream << x << " " << y << " " << *entity;
    }
    return ostream;
}

/**
 * Find an entity by location.
 * @param location The location to search.
 * @return The entity there, or null if not found.
 */
std::shared_ptr<Entity> Player::find_entity(const Location &location) const {
    if (auto entity_iterator = entities.find(location); entity_iterator != entities.end()) {
        return entity_iterator->second;
    } else {
        return std::shared_ptr<Entity>();
    }
}

/**
 * Add an entity by location, possibly merging with an existing entity.
 * @param location The location for the entity.
 * @param entity The entity to add.
 */
void Player::add_entity(const Location &location, std::shared_ptr<Entity> &entity) {
    if (auto entity_iterator = entities.find(location); entity_iterator != entities.end()) {
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

/**
 * Attempt to commit the transaction.
 * @return True if the commit succeeded.
 */
bool PlayerTransaction::commit() {
    std::set<Location> command_set;
    for (const auto &[from, to] : commands) {
        if (player.find_entity(from) == nullptr) {
            return false;
        }
        if (const auto &[_, inserted] = command_set.emplace(from); !inserted) {
            // Duplicate found
            return false;
        }
    }
    for (const auto &[from, to] : commands) {
        auto entity = player.find_entity(from);
        player.remove_entity(from);
        player.add_entity(to, entity);
    }
    return true;
}
}
