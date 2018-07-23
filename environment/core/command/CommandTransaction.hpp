#ifndef COMMANDTRANSACTION_HPP
#define COMMANDTRANSACTION_HPP

#include "Transaction.hpp"

namespace hlt {

/** Transaction for all commands. */
class CommandTransaction final : public BaseTransaction {
    /** Count of occurrences per player-entity pair, to catch duplicates. */
    std::unordered_map<Player, std::unordered_map<Entity::id_type, int>> occurrences;
    /** Total expenses per player. */
    std::unordered_map<Player, energy_type> expenses;

public:
    DumpTransaction dump_transaction;           /**< The DumpCommand transaction. */
    ConstructTransaction construct_transaction; /**< The ConstructCommand transaction. */
    MoveTransaction move_transaction;           /**< The MoveCommand transaction. */
    SpawnTransaction spawn_transaction;         /**< The SpawnCommand transaction. */

    /** All of the transactions, in commit order. */
    std::vector<std::reference_wrapper<BaseTransaction>> all_transactions
            {dump_transaction, construct_transaction, move_transaction, spawn_transaction};

    /**
     * Add a DumpCommand to the transaction.
     * @param player The player executing the command.
     * @param command The command.
     */
    void add_command(Player &player, const DumpCommand &command);

    /**
     * Add a ConstructCommand to the transaction.
     * @param player The player executing the command.
     * @param command The command.
     */
    void add_command(Player &player, const ConstructCommand &command);

    /**
     * Add a MoveCommand to the transaction.
     * @param player The player executing the command.
     * @param command The command.
     */
    void add_command(Player &player, const MoveCommand &command);

    /**
     * Add a SpawnCommand to the transaction.
     * @param player The player executing the command.
     * @param command The command.
     */
    void add_command(Player &player, const SpawnCommand &command);

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    bool check() override;

    /** If the transaction may be committed, commit the transaction. */
    void commit() override;

    /**
     * Set a callback for events generated during the transaction commit.
     * @param callback The callback to set.
     */
    void on_event(callback<GameEvent> callback) override;

    /**
     * Set a callback for errors generated in the transaction.
     * @param callback The callback to set.
     */
    void on_error(callback<CommandError> callback) override;

    /**
     * Set a callback for entity updates in the transaction.
     * @param callback The callback to set.
     */
    void on_entity_update(callback<Entity::id_type> callback) override;

    /**
     * Set a callback for cell updates in the transaction.
     * @param callback The callback to set.
     */
    void on_cell_update(callback<Location> callback) override;

    /**
     * Construct CommandTransaction from Store and map.
     * @param store The Store.
     * @param map The Map.
     */
    explicit CommandTransaction(Store &store, Map &map);
};

}

#endif // COMMANDTRANSACTION_HPP
