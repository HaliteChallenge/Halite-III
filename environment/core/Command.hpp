#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Entity.hpp"
#include "Location.hpp"
#include "Map.hpp"
#include "Player.hpp"

#include "nlohmann/json_fwd.hpp"

namespace hlt {

class CommandTransaction;

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
     * The name of the command.
     * @return The command name.
     */
    virtual Name name() const = 0;

    /**
     * Add the command to a transaction.
     * @param transaction The command transaction to act on.
     */
    virtual void add_to_transaction(CommandTransaction &transaction) const = 0;

    /** Virtual destructor. */
    virtual ~Command() = default;
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

/** Interface for commands that operate on Map and Player. */
class MapPlayerCommand : public Command {
    /**
     * Perform action on Map and Player.
     * @param map The Map.
     * @param player The Player.
     */
    virtual void act(Map &map, Player &player) const = 0;
};

/** Command for moving an entity in a direction. */
class MoveCommand : public MapPlayerCommand {
public:
    const Location entity;      /**< The location of the entity. */
    const Direction direction;  /**< The direction in which to move. */

    /**
     * Convert a MoveCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * The name of the command.
     * @return The command name.
     */
    Name name() const override { return Name::Move; }

    /**
     * Add to a transaction.
     * @param transaction The transaction.
     */
    void add_to_transaction(CommandTransaction &transaction) const override;

    /**
     * Perform the move on Map and Player.
     * @param map The Map.
     * @param player The Player.
     */
    void act(Map &map, Player &player) const override;

    /**
     * Create MoveCommand from entity and direction.
     * @param entity The location of the entity.
     * @param direction The direction.
     */
    MoveCommand(Location entity, Direction direction) : entity(entity), direction(direction) {}

};

/** Command for spawning an entity. */
class SpawnCommand : public MapPlayerCommand {
public:
    /** The amount of energy to give the new entity. */
    energy_type energy;

    /**
     * Convert a SpawnCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * The name of the command.
     * @return The command name.
     */
    Name name() const override { return Name::Spawn; }

    /**
     * Add to a transaction.
     * @param transaction The transaction.
     */
    void add_to_transaction(CommandTransaction &transaction) const override;

    /**
     * Perform the spawn on Map and Player.
     * @param map The Map.
     * @param player The Player.
     */
    void act(Map &map, Player &player) const override;

    /**
     * Create SpawnCommand from energy.
     * @param energy The energy to use.
     */
    explicit SpawnCommand(energy_type energy) : energy(energy) {}
};

class DumpCommand : public Command {
public:
    Location entity;    /**< The entity from which to dump. */
    energy_type energy; /**< The amount to dump. */

    /**
     * Convert a DumpCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * The name of the command.
     * @return The command name.
     */
    Name name() const override { return Name::Dump; }

    /**
     * Add to a transaction.
     * @param transaction The transaction.
     */
    void add_to_transaction(CommandTransaction &transaction) const override;

    /**
     * Construct DumpCommand from entity and energy.
     * @param entity The entity from which to dump.
     * @param energy The amount of energy to dump.
     */
    DumpCommand(Location entity, energy_type energy) : entity(entity), energy(energy) {}
};

/** Command to construct a drop zone. */
class ConstructCommand : public Command {
public:
    /** The entity to use to construct. */
    Location entity;

    /**
     * Convert a ConstructCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * The name of the command.
     * @return The command name.
     */
    Name name() const override { return Name::Construct; }

    /**
     * Add to a transaction.
     * @param transaction The transaction.
     */
    void add_to_transaction(CommandTransaction &transaction) const override;

    /**
     * Construct ConstructCommand from entity.
     * @param entity The entity to convert.
     */
    explicit ConstructCommand(Location entity) : entity(entity) {}
};

}

#endif // COMMAND_HPP
