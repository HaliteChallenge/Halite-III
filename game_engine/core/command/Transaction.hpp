#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <functional>
#include <unordered_set>
#include <utility>

#include "BaseTransaction.hpp"
#include "GameEvent.hpp"
#include "Location.hpp"
#include "Player.hpp"

namespace hlt {

class Command;

/**
 * Transactions that execute a series of player commands atomically.
 * @tparam Command The type of the command.
 */
template<class Command>
class Transaction : public BaseTransaction {
protected:
    using Commands = std::vector<std::reference_wrapper<const Command>>; /**< The type of the player command list. */
    id_map<Player, Commands> commands;                                   /**< The stored commands per player. */
public:
    using BaseTransaction::BaseTransaction;

    /**
     * Add a command to the transaction.
     * @param player The player executing the command.
     * @param command The command to be executed.
     */
    void add_command(Player &player, const Command &command) {
        commands[player.id].emplace_back(command);
    }

    /** Virtual destructor. */
    ~Transaction() override = default;
};

struct NoCommand {};

class ConstructCommand;

class MoveCommand;

class SpawnCommand;

/** Transaction for DumpCommand. */
class DumpTransaction final : public Transaction<NoCommand> {
public:
    using Transaction::Transaction;

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    bool check() override;

    /** If the transaction may be committed, commit the transaction. */
    void commit() override;
};

/** Transaction for ConstructCommand. */
class ConstructTransaction final : public Transaction<ConstructCommand> {
public:
    using Transaction::Transaction;

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    bool check() override;

    /** If the transaction may be committed, commit the transaction. */
    void commit() override;
};

/** Transaction for MoveCommand. */
class MoveTransaction final : public Transaction<MoveCommand> {
public:
    using Transaction::Transaction;

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    bool check() override;

    /** If the transaction may be committed, commit the transaction. */
    void commit() override;
};

/** Transaction for SpawnCommand. */
class SpawnTransaction final : public Transaction<SpawnCommand> {
public:
    using Transaction::Transaction;

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    bool check() override;

    /** If the transaction may be committed, commit the transaction. */
    void commit() override;
};

}

#endif // TRANSACTION_HPP
