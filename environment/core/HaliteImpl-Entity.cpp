#include "HaliteImpl.hpp"

namespace hlt {

/**
 * Process a spawn operation at a location, possibly merging onto an existing entity.
 * @param player The player owning the new entity.
 * @param location The location at which to spawn.
 */
void HaliteImpl::spawn_entity(Player &player, const Location &location) {
    const auto &constants = Constants::get();
    auto &entities = player.entities;
    auto entity_iterator = entities.find(location);
    if (entity_iterator != entities.end()) {
        // If there is already an entity, merge.
        entity_iterator->second->energy += constants.NEW_ENTITY_ENERGY;
    } else {
        // Otherwise, spawn.
        auto entity = make_entity<Entity>(player.player_id, constants.NEW_ENTITY_ENERGY);
        entities[location] = entity;
        game->game_map.at(location)->entities[player.player_id] = std::move(entity);
    }
}

/** Process all entity lifecycle events for this turn. */
void HaliteImpl::process_entities() {
    // Each factory may spawn a new entity
    const auto &constants = Constants::get();
    for (auto &[_, player] : game->players) {
        if (player.energy >= constants.NEW_ENTITY_ENERGY_COST) {
            player.energy -= constants.NEW_ENTITY_ENERGY_COST;
            spawn_entity(player, player.factory_location);
        }
    }
    // Each entity loses some health, each entity with no remaining energy is removed
    for (auto &[player_id, player] : game->players) {
        auto &entities = player.entities;
        for (auto entity_iterator = entities.begin(); entity_iterator != entities.end();) {
            auto [location, entity] = *entity_iterator;
            entity->energy -= constants.BASE_TURN_ENERGY_LOSS;
            if (entity->energy <= 0) {
                game->game_map.at(location)->entities.erase(player_id);
                entities.erase(entity_iterator++);
            } else {
                entity_iterator++;
            }
        }
    }
}

}
