#include "Command.hpp"
#include "Halite.hpp"

namespace hlt {

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
            if (!player.has_entity(entity_id) || player.get_entity(entity_id).energy < energy) {
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
            map.at(player.get_entity_location(command.entity)).is_dropoff = true;
            player.remove_entity(command.entity);
            player.energy -= cost;
        }
    }
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool MoveTransaction::check() {
    // TODO: implement
    return true;
}

/** If the transaction may be committed, commit the transaction. */
void MoveTransaction::commit() {
    // TODO: implement
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool SpawnTransaction::check() {
    // TODO: implement
    return true;
}

/** If the transaction may be committed, commit the transaction. */
void SpawnTransaction::commit() {
    // TODO: implement
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
