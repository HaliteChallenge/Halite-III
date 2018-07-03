#include <set>

#include "CommandTransaction.hpp"
#include "Entity.hpp"
#include "Map.hpp"

namespace hlt {

/**
 * Attempt to commit the transaction.
 * @return True if the commit succeeded.
 */
bool CommandTransaction::commit() {
    // The requested destination locations.
    std::set<Location> destinations;
    for (const auto &[from, _] : commands) {
        if (const auto &[_, inserted] = destinations.emplace(from); !inserted) {
            // Duplicate found
            return false;
        }
    }
    // Map from destination location to final energy at destination
    std::map<Location, energy_type> moved_entities;
    for (auto &[from, to] : commands) {
        // Remove each source entity and pool energies
        auto energy = _player.remove_entity(from)->energy;
        _map.at(from)->remove_entity(player);
        if (const auto &[iterator, inserted] = moved_entities.emplace(to, energy); !inserted) {
            iterator->second += energy;
        }
    }
    for (auto &[_, to] : commands) {
        if (_player.find_entity(to) != nullptr) {
            // An entity already exists at the destination, add it to pool
            auto energy = _player.remove_entity(to)->energy;
            _map.at(to)->remove_entity(player);
            moved_entities[to] += energy;
        }
    }
    for (auto &[location, energy] : moved_entities) {
        // Place new entities with corresponding energies
        auto entity = make_entity<Entity>(player.player_id, energy);
        _map.at(location)->add_entity(player, entity);
        _player.add_entity(location, std::move(entity));
    }
    return true;
}

}
