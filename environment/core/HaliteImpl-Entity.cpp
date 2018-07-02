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
        game.game_map.at(location)->entities[player.player_id] = std::move(entity);
    }
    // Create new game event for replay file, regardless of whether spawn creates a new entity or adds to an old entity
    // Ensure full frames has been initialized (ie don't do this before first turn)
    if (game.replay_struct.full_frames.size() > 0) {
        GameEvent spawn_event = std::make_unique<SpawnEvent>(location, constants.NEW_ENTITY_ENERGY, player.player_id);
        game.replay_struct.full_frames.back().events.push_back(std::move(spawn_event));
    }
}

/** Process all entity lifecycle events for this turn. */
void HaliteImpl::process_entities() {
    // Each factory may spawn a new entity
    const auto &constants = Constants::get();
    for (auto &[_, player] : game.players) {
        if (player.energy >= constants.NEW_ENTITY_ENERGY_COST) {
            player.energy -= constants.NEW_ENTITY_ENERGY_COST;
            spawn_entity(player, player.factory_location);
        }
    }
    // Each entity loses some health, each entity with no remaining energy is removed
    for (auto &[player_id, player] : game.players) {
        auto &entities = player.entities;
        for (auto entity_iterator = entities.begin(); entity_iterator != entities.end();) {
            auto [location, entity] = *entity_iterator;
            entity->energy -= constants.BASE_TURN_ENERGY_LOSS;
            if (entity->energy <= 0) {
                game.game_map.at(location)->remove_entity(player);
                // Create a new death event for replay file
                if (game.replay_struct.full_frames.size() > 0) {
                    GameEvent death_event = std::make_unique<DeathEvent>(location, entity->owner_id);
                    game.replay_struct.full_frames.back().events.push_back(std::move(death_event));
                }
                entities.erase(entity_iterator++);
            } else {
                entity_iterator++;
            }
        }
    }
}

}
