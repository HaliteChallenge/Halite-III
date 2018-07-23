#include "CommandTransaction.hpp"

namespace hlt {

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
    expenses[player] += Constants::get().NEW_ENTITY_ENERGY_COST + command.energy;
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
            error_generated<PlayerInsufficientEnergyError>(player.id, player.energy, expense);
            success = false;
        }
    }
    // Check that each entity is operated on at most once
    static constexpr auto MAX_MOVES_PER_ENTITY = 1;
    for (const auto &[player, entities] : occurrences) {
        for (const auto &[entity, number] : entities) {
            if (number > MAX_MOVES_PER_ENTITY) {
                error_generated<ExcessiveCommandsError>(player.id, entity, number);
                success = false;
            }
        }
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
