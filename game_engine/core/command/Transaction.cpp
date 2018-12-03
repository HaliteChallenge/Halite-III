#include <deque>

#include "Transaction.hpp"

#include "Halite.hpp"

namespace hlt {

/**
 * Dump energy onto a cell.
 *
 * @param store The game store.
 * @param location The location at which to dump.
 * @param cell The cell at which to dump.
 * @param energy The dumped amount of energy.
 */
void dump_energy(Store &store, const Location &location, Cell &cell, energy_type energy) {
     if (cell.owner == Player::None) {
        // Just dump directly onto the cell.
        cell.energy += energy;
        store.map_total_energy += energy;
    } else {
        // The cell owner gets all the energy
        auto& player = store.get_player(cell.owner);
        player.energy += energy;

        // Track how much energy is deposited in each dropoff
        player.total_energy_deposited += energy;
        if (location == player.factory) {
            player.factory_energy_deposited += energy;
        }
        else {
            for (auto &dropoff : player.dropoffs) {
                if (dropoff.location == location) {
                    dropoff.deposited_halite += energy;
                    return;
                }
            }
            assert(false);
        }
    }
}

void dump_energy(Store &store, Entity &entity, const Location &location, Cell &cell, energy_type energy) {
    // Decrease the entity's energy.
    entity.energy -= energy;
    dump_energy(store, location, cell, energy);
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool DumpTransaction::check() {
    return true;
}

/** If the transaction may be committed, commit the transaction. */
void DumpTransaction::commit() {
    // If an entity ends the turn on their dropoff or shipyard,
    // auto-dump all their energy.
    for (auto &[entity_id, entity] : store.all_entities()) {
        const auto &player = store.get_player(entity.owner);
        const auto &location = player.get_entity_location(entity.id);
        auto &cell = map.at(location);
        if (cell.owner == entity.owner) {
            dump_energy(store, entity, location, cell, entity.energy);
            cell_updated(location);
            entity_updated(entity.id);
        }
    }
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool ConstructTransaction::check() {
    auto success = true;
    for (const auto &[player_id, constructs] : commands) {
        auto &player = store.get_player(player_id);
        for (const ConstructCommand &command : constructs) {
            if (!player.has_entity(command.entity)) {
                // Entity is not valid
                error_generated<EntityNotFoundError<ConstructCommand>>(player_id, command);
                success = false;
            } else {
                const auto location = player.get_entity_location(command.entity);
                const auto &cell = map.at(location);
                if (cell.owner != Player::None) {
                    // Cell is already owned
                    error_generated<CellOwnedError<ConstructCommand>>(player_id, command, location, cell.owner);
                    success = false;
                }
            }
        }
    }
    return success;
}

/** If the transaction may be committed, commit the transaction. */
void ConstructTransaction::commit() {
    const auto cost = Constants::get().DROPOFF_COST;
    for (auto &[player_id, constructs] : commands) {
        auto &player = store.get_player(player_id);
        for (const ConstructCommand &command : constructs) {
            const auto entity_id = command.entity;
            const auto &entity = store.get_entity(entity_id);
            const auto location = player.get_entity_location(entity_id);
            auto &cell = map.at(location);

            // Mark as owned, clear contents of cell
            cell.owner = player_id;
            player.dropoffs.emplace_back(store.new_dropoff(location));
            store.map_total_energy -= cell.energy;

            // Cost is reduced by cargo + halite on cell
            const auto credit = cell.energy + entity.energy;

            cell.energy = 0;
            cell.entity = Entity::None;
            event_generated<ConstructionEvent>(location, player_id, command.entity);
            cell_updated(location);

            // Use dump_halite for stats tracking
            dump_energy(store, location, cell, credit);
            // Charge player
            player.energy -= cost;

            player.remove_entity(entity_id);
            store.delete_entity(entity_id);
        }
    }
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool MoveTransaction::check() {
    auto success = true;
    for (const auto &[player_id, moves] : commands) {
        auto &player = store.get_player(player_id);
        for (const MoveCommand &command : moves) {
            // Entity is not valid
            if (!player.has_entity(command.entity)) {
                error_generated<EntityNotFoundError<MoveCommand>>(player_id, command);
                success = false;
            }
        }
    }
    return success;
}

/** If the transaction may be committed, commit the transaction. */
void MoveTransaction::commit() {
    // Map from destination location to all the entities that want to go there.
    std::unordered_map<Location, std::vector<Entity::id_type>> destinations;
    /** Map from entity to the command that caused it to move. */
    id_map<Entity, std::reference_wrapper<const MoveCommand>> causes;
    // Lift each entity that is moving from the grid.
    for (auto &[player_id, moves] : commands) {
        auto &player = store.get_player(player_id);
        for (const MoveCommand &command : moves) {
            // If entity remained still, treat it as a no-op command.
            if (command.direction == Direction::Still) {
                continue;
            }
            auto location = player.get_entity_location(command.entity);
            auto &source = map.at(location);
            auto &entity = store.get_entity(command.entity);

            // Check if entity has enough energy
            const auto cost = entity.is_inspired ?
                Constants::get().INSPIRED_MOVE_COST_RATIO :
                Constants::get().MOVE_COST_RATIO;
            energy_type required = source.energy / cost;

            if (entity.energy < required) {
                // Entity does not have enough energy, ignore command.
                error_generated<InsufficientEnergyError<MoveCommand>>(player_id, command, entity.energy,
                                                                      required, !Constants::get().STRICT_ERRORS);
                continue;
            }
            causes.emplace(command.entity, command);
            // Decrease the entity's energy.
            entity.energy -= required;
            // Remove the entity from its source.
            source.entity = Entity::None;
            map.move_location(location, command.direction);
            // Mark it as interested in the destination.
            destinations[location].emplace_back(command.entity);
            // Take it from its owner.
            // Do not mark the entity as removed in the game yet.
            store.get_player(entity.owner).remove_entity(command.entity);
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
            // Destroy all interested entities and collect them in replay info
            std::vector<Entity::id_type> collision_ids;
            id_map<Player, std::vector<Entity::id_type>> self_collisions;
            id_map<Player, std::deque<std::reference_wrapper<const MoveCommand>>> self_collision_commands;
            for (auto &entity_id : entities) {
                auto &entity = store.get_entity(entity_id);
                collision_ids.push_back(entity_id);
                self_collisions[entity.owner].emplace_back(entity_id);
                if (auto cause = causes.find(entity_id); cause != causes.end()) {
                    self_collision_commands[entity.owner].emplace_back(cause->second);
                }
                // Don't delete entities/dump energy until after
                // generating the event, so that HaliteImpl has a
                // chance to collect statistics.
            }
            for (const auto &[player_id, self_collision_entities] : self_collisions) {
                if (self_collision_entities.size() > MAX_ENTITIES_PER_CELL) {
                    auto &commands = self_collision_commands[player_id];
                    const MoveCommand &first = commands.front();
                    commands.pop_front();
                    const ErrorContext context{commands.begin(), commands.end()};
                    error_generated<SelfCollisionError<MoveCommand>>(player_id, first, context, destination,
                                                                     self_collision_entities,
                                                                     !Constants::get().STRICT_ERRORS);
                }
            }

            // When generating the event, HaliteImpl will record
            // statistics.
            event_generated<CollisionEvent>(destination, collision_ids);
            // Now we can delete the entities.
            for (const auto &entity_id : collision_ids) {
                auto &entity = store.get_entity(entity_id);
                // Dump the energy.
                dump_energy(store, entity, destination, cell, entity.energy);
                store.delete_entity(entity_id);
            }

            cell_updated(destination);
        } else {
            auto &entity_id = entities.front();
            // Place it on the map.
            cell.entity = entity_id;
            // Give it back to the owner.
            store.get_player(store.get_entity(entity_id).owner).add_entity(entity_id, destination);
            entity_updated(entity_id);
        }
    }
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool SpawnTransaction::check() {
    auto success = true;
    // Only one spawn per turn
    std::unordered_set<Player::id_type> occurrences;
    static constexpr auto MAX_SPAWNS_PER_TURN = 1;
    for (const auto &[player_id, spawns] : commands) {
        if (spawns.size() > MAX_SPAWNS_PER_TURN) {
            success = false;
            std::deque<std::reference_wrapper<const Command>> spawns_deque{spawns.begin(), spawns.end()};
            // First spawn is legal
            const Command &legal = spawns_deque.front();
            spawns_deque.pop_front();
            // Second is illegal
            const Command &illegal = spawns_deque.front();
            spawns_deque.pop_front();
            // Remainder are in context
            ErrorContext context;
            context.push_back(legal);
            for (const Command &spawn : spawns_deque) {
                context.push_back(spawn);
            }
            error_generated<ExcessiveSpawnsError>(player_id, illegal, context);
        }
    }
    return success;
}

/** If the transaction may be committed, commit the transaction. */
void SpawnTransaction::commit() {
    const auto &constants = Constants::get();
    auto cost = constants.NEW_ENTITY_ENERGY_COST;
    for (const auto &[player_id, spawns] : commands) {
        for (const SpawnCommand &spawn : spawns) {
            auto &player = store.get_player(player_id);
            player.energy -= cost;
            auto &cell = map.at(player.factory);
            auto &entity = store.new_entity(0, player.id);
            player.add_entity(entity.id, player.factory);
            entity_updated(entity.id);
            event_generated<SpawnEvent>(player.factory, 0, player.id, entity.id);
            if (cell.entity == Entity::None) {
                cell.entity = entity.id;
            } else {
                // There is a collision, collide with the existing.
                auto &existing_entity = store.get_entity(cell.entity);
                auto &existing_player = store.get_player(existing_entity.owner);
                auto &owner = store.get_player(cell.owner);

                if (existing_entity.owner == cell.owner) {
                    error_generated<SelfCollisionError<SpawnCommand>>(player_id, spawn, ErrorContext(), player.factory,
                                                                      std::vector<Entity::id_type>{cell.entity, entity.id},
                                                                      !Constants::get().STRICT_ERRORS);
                }
                event_generated<CollisionEvent>(owner.factory, std::vector<Entity::id_type>{cell.entity, entity.id});

                // Use dump_energy in case the collision was from a
                // different player.
                dump_energy(store, existing_entity, owner.factory, cell, existing_entity.energy);
                existing_player.remove_entity(cell.entity);
                store.delete_entity(cell.entity);
                player.remove_entity(entity.id);
                store.delete_entity(entity.id);
                cell.entity = Entity::None;
            }
        }
    }
}

}
