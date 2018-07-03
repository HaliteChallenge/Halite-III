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
    std::map<Location, std::shared_ptr<Entity>> moved_entities;
    for (auto &[from, to] : commands) {
        auto entity = _map.at(from)->remove_entity(player);
        _player.remove_entity(from);
        if (auto entity_iterator = moved_entities.find(to); entity_iterator != moved_entities.end()) {
            // A merge is necessary
            entity_iterator->second->energy += entity->energy;
        } else {
            moved_entities[to] = std::move(entity);
        }
    }
    for (auto &[location, entity] : moved_entities) {
        _map.at(location)->add_entity(player, entity);
        _player.add_entity(location, std::move(entity));
    }
    return true;
}

}
