#include <set>

#include "Entity.hpp"
#include "Player.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, player.x}

namespace hlt {

/**
 * Convert a Player to JSON format.
 * @param[out] json The output JSON.
 * @param player The Player to convert.
 */
void to_json(nlohmann::json &json, const Player &player) {
    json = {{"player_id", player.id},
            FIELD_TO_JSON(name),
            FIELD_TO_JSON(energy),
            {"factory_location", player.factory},
            FIELD_TO_JSON(entities)};
}

/**
 * Write a Player to bot serial format.
 * @param ostream The output stream.
 * @param player The Player to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Player &player) {
    // Output player ID, number of entities, number of dropoffs, and current energy.
    ostream << player.id
            << " " << player.entities.size()
            << " " << player.dropoffs.size()
            << " " << player.energy
            << std::endl;
    return ostream;
}

/**
 * Get the location of an entity.
 * @param id The entity ID.
 * @return The entity location.
 */
Location Player::get_entity_location(const Entity::id_type &id) const {
    auto iterator = entities.find(id);
    assert(iterator != entities.end());
    return iterator->second;
}

/**
 * Get whether the player has an entity.
 * @param id The entity ID.
 * @return True if the player has the entity, false otherwise.
 */
bool Player::has_entity(const Entity::id_type &id) const {
    return entities.find(id) != entities.end();
}

/**
 * Remove an entity by ID.
 * @param id The entity ID.
 */
void Player::remove_entity(const Entity::id_type &id) {
    auto iterator = entities.find(id);
    assert(iterator != entities.end());
    entities.erase(iterator);
}

/**
 * Add an entity by ID.
 * @param id The entity ID to add.
 * @param location The location of the entity.
 */
void Player::add_entity(const Entity::id_type &id, Location location) {
    assert(!has_entity(id));
    entities.emplace(id, location);
}

}
