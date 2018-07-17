#include <unordered_set>

#include "Command.hpp"
#include "Halite.hpp"

namespace hlt {

BaseTransaction::~BaseTransaction() = default;

template<class T>
Transaction<T>::~Transaction() = default;

/**
 * Dump energy onto a cell.
 *
 * @param store The game store.
 * @param entity The entity dumping energy.
 * @param cell The cell at which to dump.
 * @param energy The dumped amount of energy.
 */
void dump_energy(Store &store, Entity &entity, Cell &cell, energy_type energy) {
    // Decrease the entity's energy.
    entity.energy -= energy;
    if (cell.owner == Player::None) {
        // Just dump directly onto the cell.
        cell.energy += energy;
    } else if (cell.owner == entity.owner) {
        // The owner gets all the energy.
        store.get_player(entity.owner).energy += energy;
    } else {
        // The energy is split.
        energy_type penalty = energy / Constants::get().DROPOFF_PENALTY_RATIO;
        energy -= penalty;
        store.get_player(entity.owner).energy += energy;
        store.get_player(cell.owner).energy += penalty;
    }
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool DumpTransaction::check() {
    for (const auto &[player_id, dumps] : commands) {
        auto &player = store.get_player(player_id);
        for (const DumpCommand &command : dumps) {
            const auto &[entity_id, energy] = command;
            // Entity is not found or has too little energy
            if (!player.has_entity(entity_id) || store.get_entity(entity_id).energy < energy) {
                offenders.emplace(player_id);
                break;
            }
        }
    }
    return offenders.empty();
}

/** If the transaction may be committed, commit the transaction. */
void DumpTransaction::commit() {
    for (const auto &[player_id, dumps] : commands) {
        auto &player = store.get_player(player_id);
        for (const DumpCommand &command : dumps) {
            auto &entity = store.get_entity(command.entity);
            const auto location = player.get_entity_location(command.entity);
            dump_energy(store, entity, map.at(location), command.energy);
            changed_cells.emplace(location);
            changed_entities.emplace(command.entity);
        }
    }
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool ConstructTransaction::check() {
    for (const auto &[player_id, constructs] : commands) {
        auto &player = store.get_player(player_id);
        for (const ConstructCommand &command : constructs) {
            // Entity is not valid or cell is already owned
            if (!player.has_entity(command.entity) ||
                map.at(player.get_entity_location(command.entity)).owner != Player::None) {
                offenders.emplace(player_id);
                break;
            }
        }
    }
    return offenders.empty();
}

/** If the transaction may be committed, commit the transaction. */
void ConstructTransaction::commit() {
    auto cost = Constants::get().DROPOFF_COST;
    for (auto &[player_id, constructs] : commands) {
        auto &player = store.get_player(player_id);
        for (const ConstructCommand &command : constructs) {
            const auto entity = command.entity;
            const auto location = player.get_entity_location(entity);
            auto &cell = map.at(location);
            // Mark as owned, clear contents of cell
            cell.owner = player_id;
            player.dropoffs.emplace_back(location);
            cell.energy = 0;
            cell.entity = Entity::None;
            changed_cells.emplace(location);
            player.remove_entity(entity);
            store.delete_entity(entity);
            // Charge player
            player.energy -= cost;
        }
    }
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool MoveTransaction::check() {
    auto cost = Constants::get().MOVE_COST_RATIO;
    for (const auto &[player_id, moves] : commands) {
        auto &player = store.get_player(player_id);
        for (const MoveCommand &command : moves) {
            // Entity is not valid
            if (!player.has_entity(command.entity)) {
                offenders.emplace(player_id);
                break;
            }
            // Entity does not have enough energy
            energy_type required = map.at(player.get_entity_location(command.entity)).energy / cost;
            if (store.get_entity(command.entity).energy < required) {
                offenders.emplace(player_id);
                break;
            }
        }
    }
    return offenders.empty();
}

/** If the transaction may be committed, commit the transaction. */
void MoveTransaction::commit() {
    auto cost = Constants::get().MOVE_COST_RATIO;
    // Map from destination location to all the entities that want to go there.
    std::unordered_map<Location, std::vector<Entity::id_type>> destinations;
    // Lift each entity that is moving from the grid.
    for (auto &[player_id, moves] : commands) {
        auto &player = store.get_player(player_id);
        for (const MoveCommand &command : moves) {
            const auto &[entity_id, direction] = command;
            auto location = player.get_entity_location(entity_id);
            auto &source = map.at(location);
            // Decrease the entity's energy.
            store.get_entity(entity_id).energy -= source.energy / cost;
            // Remove the entity from its source.
            source.entity = Entity::None;
            map.move_location(location, direction);
            // Mark it as interested in the destination.
            destinations[location].emplace_back(entity_id);
            // Take it from its owner.
            // Do not mark the entity as removed in the game yet.
            store.get_player(store.get_entity(entity_id).owner).remove_entity(entity_id);
        }
    }
    // If there are already unmoving entities at the destination, lift them off too.
    for (auto &[destination, _] : destinations) {
        auto &cell = map.at(destination);
        if (cell.entity != Entity::None) {
            destinations[destination].emplace_back(cell.entity);
            store.get_player(store.get_entity(cell.entity).owner).remove_entity(cell.entity);
            cell.entity = Entity::None;
        }
    }
    // If only one entity is interested in a destination, place it there.
    // Otherwise, destroy all interested entities.
    static constexpr auto MAX_ENTITIES_PER_CELL = 1;
    for (auto &[destination, entities] : destinations) {
        auto &cell = map.at(destination);
        if (entities.size() > MAX_ENTITIES_PER_CELL) {
            // Destroy all interested entities.
            for (auto &entity_id : entities) {
                // Dump the energy.
                auto &entity = store.get_entity(entity_id);
                dump_energy(store, entity, cell, entity.energy);
                store.delete_entity(entity_id);
            }
            changed_cells.emplace(destination);
        } else {
            auto &entity_id = entities.front();
            // Place it on the map.
            cell.entity = entity_id;
            // Give it back to the owner.
            store.get_player(store.get_entity(entity_id).owner).add_entity(entity_id, destination);
            changed_entities.emplace(entity_id);
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
    for (const auto &[player_id, _] : commands) {
        if (const auto &[__, inserted] = occurrences.emplace(player_id); !inserted) {
            offenders.emplace(player_id);
        }
    }
    return offenders.empty();
}

/** If the transaction may be committed, commit the transaction. */
void SpawnTransaction::commit() {
    const auto &constants = Constants::get();
    auto cost = constants.NEW_ENTITY_ENERGY_COST;
    auto energy = constants.NEW_ENTITY_ENERGY;
    for (const auto &[player_id, _] : commands) {
        auto &player = store.get_player(player_id);
        player.energy -= cost;
        auto &cell = map.at(player.factory);
        if (cell.entity == Entity::None) {
            auto &entity = store.new_entity(energy, player.id);
            player.add_entity(entity.id, player.factory);
            cell.entity = entity.id;
            changed_entities.emplace(entity.id);
        } else {
            // There is a collision, destroy the existing.
            store.get_player(store.get_entity(cell.entity).owner).remove_entity(cell.entity);
            store.delete_entity(cell.entity);
            cell.entity = Entity::None;
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
    spawn_transaction.add_command(player, command);
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool CommandTransaction::check() {
    bool success = true;
    // Check that expenses are not too high
    for (const auto &[player, expense] : expenses) {
        if (expense > player.energy) {
            offenders.emplace(player.id);
            success = false;
        }
    }
    // Check that each entity is operated on at most once
    static constexpr auto MAX_MOVES_PER_ENTITY = 1;
    for (const auto &[player, entities] : occurrences) {
        for (const auto &[_, number] : entities) {
            if (number > MAX_MOVES_PER_ENTITY) {
                offenders.emplace(player.id);
                success = false;
            }
        }
    }
    // Check that each transaction can succeed individually
    for (BaseTransaction &transaction : all_transactions) {
        if (!transaction.check()) {
            offenders.insert(transaction.offenders.begin(), transaction.offenders.end());
            success = false;
        }
    }
    return success;
}

/** If the transaction may be committed, commit the transaction. */
void CommandTransaction::commit() {
    for (BaseTransaction &transaction : all_transactions) {
        transaction.commit();
        changed_entities.insert(transaction.changed_entities.begin(), transaction.changed_entities.end());
        changed_cells.insert(transaction.changed_cells.begin(), transaction.changed_cells.end());
    }
}

}
