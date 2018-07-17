#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Entity.hpp"
#include "Location.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Transaction.hpp"

#include "nlohmann/json_fwd.hpp"

namespace hlt {

/** Abstract type of commands issued by the user. */
class Command {
public:
    /** The command names. */
    enum Name : char {
        Move = 'm',
        Spawn = 'g',
        Dump = 'd',
        Construct = 'c'
    };

    /**
     * Convert to JSON format.
     * @param[out] json The JSON output.
     */
    virtual void to_json(nlohmann::json &json) const = 0;

    /**
     * Add the command to a transaction.
     * @param player The player executing the command.
     * @param transaction The command transaction to act on.
     */
    virtual void add_to_transaction(Player &player, CommandTransaction &transaction) const = 0;

    /** Virtual destructor. */
    virtual ~Command() = default;
};

/** Statically polymorphic mixin for commands that may be added to a transaction. */
template<class T>
class TransactableCommand : public Command {
public:
    /**
     * Add the command to a transaction.
     * @param player The player executing the command.
     * @param transaction The command transaction to act on.
     */
    void add_to_transaction(Player &player, CommandTransaction &transaction) const final {
        // Invoke overload resolution on CommandTransaction::add_command
        transaction.add_command(player, static_cast<const T &>(*this));
    }

    /** Virtual destructor. */
    ~TransactableCommand() override = 0;
};

/**
 * Convert a Command to JSON format.
 * @param[out] json The output JSON.
 * @param command The command to convert.
 */
void to_json(nlohmann::json &json, const Command &command);

/**
 * Read a Command from bot serial format.
 * @param istream The input stream.
 * @param[out] command The command to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, std::unique_ptr<Command> &command);

/** Command for moving an entity in a direction. */
class MoveCommand final : public TransactableCommand<MoveCommand> {
public:
    const Entity::id_type entity; /**< The location of the entity. */
    const Direction direction;    /**< The direction in which to move. */

    /**
     * Convert a MoveCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Create MoveCommand from entity and direction.
     * @param entity The location of the entity.
     * @param direction The direction.
     */
    MoveCommand(const Entity::id_type &entity, Direction direction) : entity(entity), direction(direction) {}
};

/** Command for spawning an entity. */
class SpawnCommand final : public TransactableCommand<SpawnCommand> {
public:
    /** The amount of energy to give the new entity. */
    const energy_type energy;

    /**
     * Convert a SpawnCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Create SpawnCommand from energy.
     * @param energy The energy to use.
     */
    explicit SpawnCommand(energy_type energy) : energy(energy) {}
};

class DumpCommand final : public TransactableCommand<DumpCommand> {
public:
    const Entity::id_type entity; /**< The entity from which to dump. */
    const energy_type energy;     /**< The amount to dump. */

    /**
     * Convert a DumpCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Construct DumpCommand from entity and energy.
     * @param entity The entity from which to dump.
     * @param energy The amount of energy to dump.
     */
    DumpCommand(const Entity::id_type &entity, energy_type energy) : entity(entity), energy(energy) {}
};

/** Command to construct a drop zone. */
class ConstructCommand final : public TransactableCommand<ConstructCommand> {
public:
    /** The entity to use to construct. */
    const Entity::id_type entity;

    /**
     * Convert a ConstructCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Construct ConstructCommand from entity.
     * @param entity The entity to convert.
     */
    explicit ConstructCommand(const Entity::id_type &entity) : entity(entity) {}
};

}

#endif // COMMAND_HPP
