#include "Store.hpp"

namespace hlt {

/**
 * Get a player by ID.
 *
 * @param id The player ID.
 * @return The player.
 */
Player &Store::get_player(const Player::id_type &id) {
    return players.find(id)->second;
}

/**
 * Get an entity by ID.
 *
 * @param id The entity ID.
 * @return The entity.
 */
Entity &Store::get_entity(const Entity::id_type &id) {
    return entities.find(id)->second;
}

/**
 * Obtain a new entity.
 *
 * @param energy The energy of the entity.
 * @param owner The owner of the entity.
 * @return The new entity.
 */
Entity &Store::new_entity(energy_type energy, const Player::id_type &owner) {
    auto entity = entity_factory.make(owner, energy);
    return entities.emplace(entity.id, entity).first->second;
}

/**
 * Delete an entity by ID.
 *
 * @param id The ID of the entity.
 */
void Store::delete_entity(const Entity::id_type id) {
    entities.erase(id);
}

}
