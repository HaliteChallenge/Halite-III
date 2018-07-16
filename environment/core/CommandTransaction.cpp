#include "Command.hpp"

namespace hlt {

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool DumpTransaction::check() const {
    // TODO: implement
    return true;
}

/** If the transaction may be committed, commit the transaction. */
void DumpTransaction::commit() {
    // TODO: implement
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool ConstructTransaction::check() const {
    // TODO: implement
    return true;
}

/** If the transaction may be committed, commit the transaction. */
void ConstructTransaction::commit() {
    // TODO: implement
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool MoveTransaction::check() const {
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
bool SpawnTransaction::check() const {
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
    dump_transaction.add_command(player, command);
}

/**
 * Add a ConstructCommand to the transaction.
 * @param player The player executing the command.
 * @param command The command.
 */
void CommandTransaction::add_command(Player &player, const ConstructCommand &command) {
    construct_transaction.add_command(player, command);
}

/**
 * Add a MoveCommand to the transaction.
 * @param player The player executing the command.
 * @param command The command.
 */
void CommandTransaction::add_command(Player &player, const MoveCommand &command) {
    move_transaction.add_command(player, command);
}

/**
 * Add a SpawnCommand to the transaction.
 * @param player The player executing the command.
 * @param command The command.
 */
void CommandTransaction::add_command(Player &player, const SpawnCommand &command) {
    spawn_transaction.add_command(player, command);
}

/**
 * Check if the transaction may be committed without actually committing.
 * @return False if the transaction may not be committed.
 */
bool CommandTransaction::check() const {
    for (const BaseTransaction &transaction : all_transactions) {
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
