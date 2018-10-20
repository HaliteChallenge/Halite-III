#include "CommandTransaction.hpp"
#include "Store.hpp"

namespace hlt {

/** The maximum number of commands per entity. */
static constexpr auto MAX_COMMANDS_PER_ENTITY = 1;

/**
 * Check that a command operates on an entity owned by the player.
 * @param player The player.
 * @param entity The entity.
 * @param command The command.
 */
bool CommandTransaction::check_ownership(const Player &player, Entity::id_type entity, const MoveCommand &command) {
    if (!player.has_entity(entity)) {
        move_ownership_faulty[player.id].emplace_back(command);
        return false;
    }
    return true;
}
bool CommandTransaction::check_ownership(const Player &player, Entity::id_type entity, const ConstructCommand &command) {
    if (!player.has_entity(entity)) {
        construct_ownership_faulty[player.id].emplace_back(command);
        return false;
    }
    return true;
}


/**
 * Add a command occurrence for an entity.
 * @param entity The entity.
 * @param command The command.
 */
void CommandTransaction::add_occurrence(Entity::id_type entity, const Command &command) {
    auto &occurrences_entry = occurrences[entity];
    if (occurrences_entry.first++ == MAX_COMMANDS_PER_ENTITY) {
        // Already seen one entity, this one is illegal
        occurrences_first_faulty.emplace(entity, command);
    } else {
        // Not yet seen or already found illegal, this one is context
        occurrences_entry.second.emplace_back(command);
    }
}

/**
 * Add an expense for a player.
 * @param player The player.
 * @param command The command.
 * @param amount The expense amount.
 */
void CommandTransaction::add_expense(const Player &player, const Command &command, energy_type amount) {
    auto &expenses_entry = expenses[player.id];
    if ((expenses_entry.first += amount) > player.energy) {
        if (auto [_, inserted] = expenses_first_faulty.emplace(player.id, command); !inserted) {
            // This one is context
            expenses_entry.second.emplace_back(command);
        }
    } else {
        // This one is legal
        expenses_entry.second.emplace_back(command);
    }
}

/**
 * Add a ConstructCommand to the transaction.
 * @param player The player executing the command.
 * @param command The command.
 */
void CommandTransaction::add_command(Player &player, const ConstructCommand &command) {
    if (check_ownership(player, command.entity, command)) {
        add_occurrence(command.entity, command);
        auto cost = Constants::get().DROPOFF_COST;

        // Cost factors in entity cargo and halite on target cell.
        const auto location = player.get_entity_location(command.entity);
        const auto &cell = map.at(location);
        const auto &entity = store.get_entity(command.entity);
        if (cell.energy + entity.energy >= cost) {
            cost = 0;
        }
        else {
            cost -= cell.energy + entity.energy;
        }
        add_expense(player, command, cost);

        construct_transaction.add_command(player, command);
    }
}

/**
 * Add a MoveCommand to the transaction.
 * @param player The player executing the command.
 * @param command The command.
 */
void CommandTransaction::add_command(Player &player, const MoveCommand &command) {
    if (check_ownership(player, command.entity, command)) {
        add_occurrence(command.entity, command);
        move_transaction.add_command(player, command);
    }
}

/**
 * Add a SpawnCommand to the transaction.
 * @param player The player executing the command.
 * @param command The command.
 */
void CommandTransaction::add_command(Player &player, const SpawnCommand &command) {
    add_expense(player, command, Constants::get().NEW_ENTITY_ENERGY_COST);
    spawn_transaction.add_command(player, command);
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool CommandTransaction::check() {
    bool success = true;
    // Check that player didn't try to command enemy ships
    for (const auto &[player_id, misowned] : move_ownership_faulty) {
        for (const auto &faulty : misowned) {
            error_generated<EntityNotFoundError<MoveCommand>>(player_id, faulty);
        }
        success = false;
    }
    for (const auto &[player_id, misowned] : construct_ownership_faulty) {
        for (const auto &faulty : misowned) {
            error_generated<EntityNotFoundError<ConstructCommand>>(player_id, faulty);
        }
        success = false;
    }

    // Check that expenses are not too high
    for (auto &[player_id, faulty] : expenses_first_faulty) {
        const auto &player = store.get_player(player_id);
        auto &[energy, context] = expenses[player_id];
        error_generated<PlayerInsufficientEnergyError>(player_id, faulty, context, player.energy, energy);
        success = false;
    }
    // Check that each entity is operated on at most once
    for (auto &[entity_id, faulty] : occurrences_first_faulty) {
        const auto owner = store.get_entity(entity_id).owner;
        auto &[_, context] = occurrences[entity_id];
        error_generated<ExcessiveCommandsError>(owner, faulty, context, entity_id);
        success = false;
    }
    // Check that each transaction can succeed individually
    for (BaseTransaction &transaction : all_transactions) {
        if (!transaction.check()) {
            success = false;
        }
    }
    return success;
}

/** If the transaction may be committed, commit the transaction. */
void CommandTransaction::commit() {
    for (BaseTransaction &transaction : all_transactions) {
        transaction.commit();
    }
}

/**
 * Set a callback for events generated during the transaction commit.
 * @param callback The callback to set.
 */
void CommandTransaction::on_event(callback <GameEvent> callback) {
    for (BaseTransaction &transaction : all_transactions) {
        transaction.on_event(callback);
    }
    BaseTransaction::on_event(callback);
}

/**
 * Set a callback for errors generated in the transaction.
 * @param callback The callback to set.
 */
void CommandTransaction::on_error(callback <CommandError> callback) {
    for (BaseTransaction &transaction : all_transactions) {
        transaction.on_error(callback);
    }
    BaseTransaction::on_error(callback);
}

/**
 * Set a callback for entity updates in the transaction.
 * @param callback The callback to set.
 */
void CommandTransaction::on_entity_update(callback <Entity::id_type> callback) {
    for (BaseTransaction &transaction : all_transactions) {
        transaction.on_entity_update(callback);
    }
    BaseTransaction::on_entity_update(callback);
}

/**
 * Set a callback for cell updates in the transaction.
 * @param callback The callback to set.
 */
void CommandTransaction::on_cell_update(callback <Location> callback) {
    for (BaseTransaction &transaction : all_transactions) {
        transaction.on_cell_update(callback);
    }
    BaseTransaction::on_cell_update(callback);
}

/**
 * Construct CommandTransaction from Store and map.
 * @param store The Store.
 * @param map The Map.
 */
CommandTransaction::CommandTransaction(Store &store, Map &map) :
        BaseTransaction(store, map),
        dump_transaction(store, map),
        construct_transaction(store, map),
        move_transaction(store, map),
        spawn_transaction(store, map) {}

}
