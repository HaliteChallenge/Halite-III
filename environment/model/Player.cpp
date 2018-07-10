#include <set>

#include "Entity.hpp"

#include "nlohmann/json.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, player.x}

/** Get a field from JSON. */
#define FIELD_FROM_JSON(x) json.at(#x)

/** The JSON key for factory. */
constexpr auto JSON_FACTORY_KEY = "factory_location";

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
    json = {FIELD_TO_JSON(player_id),
            FIELD_TO_JSON(name),
            FIELD_TO_JSON(energy),
            {JSON_FACTORY_KEY, player.factories.front()},
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
std::shared_ptr<PlayerEntity> Player::find_entity(const Location &location) const {
    if (auto entity_iterator = entities.find(location); entity_iterator != entities.end()) {
        return entity_iterator->second;
    } else {
        return std::shared_ptr<PlayerEntity>();
    }
}

/**
 * Add a new entity by location. No entity must exist at that location.
 * @param location The location for the entity.
 * @param entity The entity to add.
 */
void Player::add_entity(const Location &location, std::shared_ptr<PlayerEntity> entity) {
    entities[location] = entity;
}

/**
 * Remove an entity by location.
 * @param location The location of the entity.
 * @return The entity there.
 */
std::shared_ptr<PlayerEntity> Player::remove_entity(const Location &location) {
    auto found = std::move(entities[location]);
    entities.erase(location);
    return found;
}

void Player::log_error_section(const std::string& section_name) {
    error_log.append(section_name);
    error_log.append("\n");
    error_log.append("================================================================\n");
}

void Player::log_error(const std::string& text) {
    error_log.append(text);
    error_log.append("\n");
}

}
