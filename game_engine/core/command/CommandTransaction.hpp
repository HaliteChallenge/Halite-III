#ifndef COMMANDTRANSACTION_HPP
#define COMMANDTRANSACTION_HPP

#include "Transaction.hpp"

namespace hlt {

/** Transaction for all commands. */
class CommandTransaction final : public BaseTransaction {
    /** Command occurrences per entity, to catch duplicates. */
    id_map<Entity, std::pair<int, ErrorContext>> occurrences;
    /** First command which broke occurrences requirement. */
    id_map<Entity, std::reference_wrapper<const Command>> occurrences_first_faulty;
    /** Total expenses per player. */
    id_map<Player, std::pair<energy_type, ErrorContext>> expenses;
    /** First command which broke expense requirement. */
    id_map<Player, std::reference_wrapper<const Command>> expenses_first_faulty;
    /** Commands which break ownership requirement. */
    id_map<Player, std::vector<std::reference_wrapper<const MoveCommand>>> move_ownership_faulty;
    /** Commands which break ownership requirement. */
    id_map<Player, std::vector<std::reference_wrapper<const ConstructCommand>>> construct_ownership_faulty;

    /**
     * Check that a command operates on an entity owned by the player.
     * @param player The player.
     * @param entity The entity.
     * @param command The command.
     */
    // this is a terrible way to do it, I apologize
    bool check_ownership(const Player &player, Entity::id_type entity, const MoveCommand &command);
    bool check_ownership(const Player &player, Entity::id_type entity, const ConstructCommand &command);

    /**
     * Add a command occurrence for an entity.
     * @param entity The entity.
     * @param command The command.
     */
    void add_occurrence(Entity::id_type entity, const Command &command);

    /**
     * Add an expense for a player.
     * @param player The player.
     * @param command The command.
     * @param amount The expense amount.
     */
    void add_expense(const Player &player, const Command &command, energy_type amount);

public:
    DumpTransaction dump_transaction;           /**< The transaction for auto-dumping. */
    ConstructTransaction construct_transaction; /**< The ConstructCommand transaction. */
    MoveTransaction move_transaction;           /**< The MoveCommand transaction. */
    SpawnTransaction spawn_transaction;         /**< The SpawnCommand transaction. */

    /** All of the transactions, in commit order. */
    std::vector<std::reference_wrapper<BaseTransaction>> all_transactions
        {construct_transaction, move_transaction, dump_transaction, spawn_transaction};

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
