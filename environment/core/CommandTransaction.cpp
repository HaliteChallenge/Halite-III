#include <unordered_set>

#include "CommandTransaction.hpp"
#include "Map.hpp"

namespace hlt {

/**
 * Attempt to commit the transaction.
 * @return True if the commit succeeded.
 */
bool CommandTransaction::commit() {
    // The requested destination locations.
    std::unordered_set<Location> destinations;
    for (const auto &[from, _] : move_commands) {
        if (const auto &[_, inserted] = destinations.emplace(from); !inserted) {
            // Duplicate found
            return false;
        }
    }

    // See if attempting to spawn multiple pieces from one factory, or insufficient energy
    std::unordered_set<Location> factories;
    const auto &constants = Constants::get();
    for (const auto &[factory, energy] : spawn_commands) {
        if (energy > constants.MAX_ENERGY || player.energy < energy * constants.NEW_ENTITY_ENERGY_COST) {
            // Insufficient energy
            return false;
        }
        if (const auto &[_, inserted] = factories.emplace(factory); !inserted) {
            // Duplicate found
            return false;
        }
    }

    // Process move events
    // Map from destination location to final energy at destination
    std::unordered_map<Location, energy_type> moved_entities;
    for (auto &[from, to] : move_commands) {
        // Remove each source entity and pool energies
        auto energy = _player.remove_entity(from)->energy;
        _map.at(from)->remove_entity(player);
        if (const auto &[iterator, inserted] = moved_entities.emplace(to, energy); !inserted) {
            iterator->second += energy;
        }
    }
    for (auto &[_, to] : move_commands) {
        if (_player.find_entity(to) != nullptr) {
            // An entity already exists at the destination, add it to pool
            auto energy = _player.remove_entity(to)->energy;
            _map.at(to)->remove_entity(player);
            moved_entities[to] += energy;
        }
    }
    for (auto &[location, energy] : moved_entities) {
        // Place new entities with corresponding energies
        auto entity = make_entity<PlayerEntity>(player.player_id, energy);
        _map.at(location)->add_entity(player, entity);
        _player.add_entity(location, std::move(entity));
    }

    // Process spawn events
    for (const auto &[factory, energy] : spawn_commands) {
        auto &entities = _player.entities;
        if (auto entity_iterator = entities.find(factory); entity_iterator != entities.end()) {
            // If there is already an entity, merge.
            entity_iterator->second->energy += energy;
        } else {
            // Otherwise, spawn.
            auto entity = make_entity<PlayerEntity>(player.player_id, energy);
            entities[factory] = entity;
            _map.at(factory)->entities[player.player_id] = std::move(entity);
        }
        _player.energy -= energy * constants.NEW_ENTITY_ENERGY_COST;
        if (callback) {
            callback(std::make_unique<SpawnEvent>(factory, energy, player.player_id));
        }
    }
    return true;
}

/**
 * Set a callback for GameEvents generated during the transaction commit.
 * @param callback The callback to set.
 */
void CommandTransaction::set_callback(std::function<void(GameEvent)> callback) {
    this->callback = callback;
}

}
