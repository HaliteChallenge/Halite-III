#include <unordered_set>

#include "Command.hpp"
#include "Halite.hpp"

namespace hlt {

BaseTransaction::~BaseTransaction() = default;

template<class T>
Transaction<T>::~Transaction() = default;

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool DumpTransaction::check() {
    for (const auto &[player_id, dumps] : commands) {
        auto &player = game.get_player(player_id);
        for (const DumpCommand &command : dumps) {
            const auto &[entity_id, energy] = command;
            // Entity is not found or has too little energy
            if (!player.has_entity(entity_id) || game.get_entity(entity_id).energy < energy) {
                _offender = player_id;
                return false;
            }
        }
    }
    return true;
}

/** If the transaction may be committed, commit the transaction. */
void DumpTransaction::commit() {
    for (const auto &[player_id, dumps] : commands) {
        auto &player = game.get_player(player_id);
        for (const DumpCommand &command : dumps) {
            auto &entity = game.get_entity(command.entity);
            entity.energy -= command.energy;
            Location location = player.get_entity_location(command.entity);
            // TODO: if location is factory or drop zone, act accordingly
            map.at(location).energy += command.energy;
        }
    }
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool ConstructTransaction::check() {
    for (const auto &[player_id, constructs] : commands) {
        auto &player = game.get_player(player_id);
        for (const ConstructCommand &command : constructs) {
            // Entity is not valid
            if (!player.has_entity(command.entity)) {
                _offender = player_id;
                return false;
            }
        }
    }
    return true;
}

/** If the transaction may be committed, commit the transaction. */
void ConstructTransaction::commit() {
    auto cost = Constants::get().DROPOFF_COST;
    for (auto &[player_id, constructs] : commands) {
        auto &player = game.get_player(player_id);
        for (const ConstructCommand &command : constructs) {
            auto location = player.get_entity_location(command.entity);
            auto &cell = map.at(location);
            cell.is_dropoff = true;
            cell.energy = 0;
            cell.entity = Entity::None;
            if (callback) {
                callback(std::make_unique<ConstructionEvent>(location, player_id, command.entity));
            }
            player.remove_entity(command.entity);
            game.delete_entity(command.entity);
            player.energy -= cost;
        }
    }
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool MoveTransaction::check() {
    for (const auto &[player_id, moves] : commands) {
        auto &player = game.get_player(player_id);
        for (const MoveCommand &command : moves) {
            // Entity is not valid
            if (!player.has_entity(command.entity)) {
                _offender = player_id;
                return false;
            }
        }
        // TODO: check that the energy is sufficient to move off the source
    }
    return true;
}

/** If the transaction may be committed, commit the transaction. */
void MoveTransaction::commit() {
    // Map from destination location to all the entities that want to go there.
    std::unordered_map<Location, std::vector<Entity::id_type>> destinations;
    // Lift each entity that is moving from the grid.
    for (auto &[player_id, moves] : commands) {
        auto &player = game.get_player(player_id);
        for (const MoveCommand &command : moves) {
            const auto &[entity_id, direction] = command;
            auto location = player.get_entity_location(entity_id);
            // Remove the entity from its source
            auto &source = map.at(location);
            source.entity = Entity::None;
            map.move_location(location, direction);
            // Mark it as interested in the destination.
            destinations[location].emplace_back(entity_id);
            // Take it from its owner.
            // Do not mark the entity as removed in the game yet.
            game.get_owner(entity_id).remove_entity(entity_id);
        }
    }
    // If there are already unmoving entities at the destination, lift them off too.
    for (auto &[destination, _] : destinations) {
        auto &cell = map.at(destination);
        if (cell.entity != Entity::None) {
            destinations[destination].emplace_back(cell.entity);
            game.get_owner(cell.entity).remove_entity(cell.entity);
            cell.entity = Entity::None;
        }
    }
    // If only one entity is interested in a destination, place it there.
    // Otherwise, destroy all interested entities.
    static constexpr auto MAX_ENTITIES_PER_CELL = 1;
    for (auto &[destination, entities] : destinations) {
        if (entities.size() > MAX_ENTITIES_PER_CELL) {
            // Destroy all interested entities and collect them in replay info
            std::vector<Entity::id_type> collision_ids;
            for (auto &entity_id : entities) {
                collision_ids.push_back(entity_id);
                game.delete_entity(entity_id);
            }
            if (callback) {
                callback(std::make_unique<CollisionEvent>(destination, collision_ids));
            }
        } else {
            auto &entity_id = entities.front();
            // Place it on the map.
            map.at(destination).entity = entity_id;
            // Give it back to the owner.
            game.get_owner(entity_id).add_entity(entity_id, destination);
            // TODO: decrease energy on the moved entity
        }
    }
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool SpawnTransaction::check() {
    // Only one spawn per turn
    std::unordered_set<Player::id_type> occurrences;
    for (const auto &[player, _] : commands) {
        if (const auto &[__, inserted] = occurrences.emplace(player); !inserted) {
            _offender = player;
            return false;
        }
    }
    return true;
}

/** If the transaction may be committed, commit the transaction. */
void SpawnTransaction::commit() {
    const auto &constants = Constants::get();
    auto cost = constants.NEW_ENTITY_ENERGY_COST;
    for (const auto &[player_id, spawns] : commands) {
        for (const SpawnCommand &spawn : spawns) {
            auto &player = game.get_player(player_id);
            auto energy = spawn.energy;
            player.energy -= (cost + energy);
            auto &cell = map.at(player.factory);
            if (cell.entity == Entity::None) {
                cell.entity = game.new_entity(energy, player, player.factory).id;
                if (callback) {
                    callback(std::make_unique<SpawnEvent>(player.factory, energy, player_id, cell.entity));
                }
            } else {
                // There is a collision
                auto &owner = game.get_owner(cell.entity);
                owner.remove_entity(cell.entity);
                if (callback) {
                    std::vector<Entity::id_type> collision_ids = {cell.entity};
                    callback(std::make_unique<CollisionEvent>(player.factory, collision_ids));
                }
                game.delete_entity(cell.entity);
                cell.entity = Entity::None;
            }
        }
    }
}

/**
 * Add a DumpCommand to the transaction.
 * @param player The player executing the command.
 * @param command The command.
 */
void CommandTransaction::add_command(Player &player, const DumpCommand &command) {
    occurrences[player][command.entity]++;
    dump_transaction.add_command(player, command);
}

/**
 * Add a ConstructCommand to the transaction.
 * @param player The player executing the command.
 * @param command The command.
 */
void CommandTransaction::add_command(Player &player, const ConstructCommand &command) {
    occurrences[player][command.entity]++;
    expenses[player] += Constants::get().DROPOFF_COST;
    construct_transaction.add_command(player, command);
}

/**
 * Add a MoveCommand to the transaction.
 * @param player The player executing the command.
 * @param command The command.
 */
void CommandTransaction::add_command(Player &player, const MoveCommand &command) {
    occurrences[player][command.entity]++;
    move_transaction.add_command(player, command);
}

/**
 * Add a SpawnCommand to the transaction.
 * @param player The player executing the command.
 * @param command The command.
 */
void CommandTransaction::add_command(Player &player, const SpawnCommand &command) {
    expenses[player] += Constants::get().NEW_ENTITY_ENERGY_COST;
    expenses[player] += command.energy;
    spawn_transaction.add_command(player, command);
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool CommandTransaction::check() {
    // Check that expenses are not too high
    for (const auto &[player, expense] : expenses) {
        if (expense > player.energy) {
            _offender = player.id;
            return false;
        }
    }
    // Check that each entity is operated on at most once
    static constexpr auto MAX_MOVES_PER_ENTITY = 1;
    for (const auto &[player, entities] : occurrences) {
        for (const auto &[_, number] : entities) {
            if (number > MAX_MOVES_PER_ENTITY) {
                _offender = player.id;
                return false;
            }
        }
    }
    // Check that each transaction can succeed individually
    for (BaseTransaction &transaction : all_transactions) {
        if (!transaction.check()) {
            return false;
        }
    }
    return true;
}

/** If the transaction may be committed, commit the transaction. */
void CommandTransaction::commit() {
    for (BaseTransaction &transaction : all_transactions) {
        transaction.commit();
    }
}

}
