#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "CommandTransaction.hpp"
#include "Entity.hpp"
#include "Location.hpp"
#include "Map.hpp"
#include "Player.hpp"

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
class Transactable : public virtual Command {
    /**
     * Add the command to a transaction.
     * @param player The player executing the command.
     * @param transaction The command transaction to act on.
     */
    void add_to_transaction(Player &player, CommandTransaction &transaction) const override {
        // Invoke overload resolution on CommandTransaction::add_command
        transaction.add_command(player, static_cast<const T &>(*this));
    }
};

/**
 * Convert a Command to JSON format.
 * @param[out] json The output JSON.
 * @param command The command to convert.
 */
void to_json(nlohmann::json &json, const Command &command);

/**
 * Convert a Command ptr to JSON format.
 * @param[out] json The output JSON.
 * @param command The command to convert.
 */
void to_json(nlohmann::json &json, const std::unique_ptr<Command> &command);

/**
 * Read a Command from bot serial format.
 * @param istream The input stream.
 * @param[out] command The command to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, std::unique_ptr<Command> &command);

/** Command for moving an entity in a direction. */
class MoveCommand : public virtual Command, public Transactable<MoveCommand> {
public:
    const Location entity;      /**< The location of the entity. */
    const Direction direction;  /**< The direction in which to move. */

    /**
     * Convert a MoveCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Perform the move on Map and Player.
     * @param map The Map.
     * @param player The Player.
     */
    void act(Map &map, Player &player) const;

    /**
     * Create MoveCommand from entity and direction.
     * @param entity The location of the entity.
     * @param direction The direction.
     */
    MoveCommand(Location entity, Direction direction) : entity(entity), direction(direction) {}
};

/** Command for spawning an entity. */
class SpawnCommand : public virtual Command, public Transactable<SpawnCommand> {
public:
    /** The amount of energy to give the new entity. */
    const energy_type energy;

    /**
     * Convert a SpawnCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Perform the spawn on Map and Player.
     * @param map The Map.
     * @param player The Player.
     */
    void act(Map &map, Player &player) const;

    /**
     * Create SpawnCommand from energy.
     * @param energy The energy to use.
     */
    explicit SpawnCommand(energy_type energy) : energy(energy) {}
};

class DumpCommand : public virtual Command, public Transactable<DumpCommand> {
public:
    const Location entity;    /**< The entity from which to dump. */
    const energy_type energy; /**< The amount to dump. */

    /**
     * Convert a DumpCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Perform the dump on the Map.
     * @param map The Map.
     */
    void act(const Map &map) const;

    /**
     * Construct DumpCommand from entity and energy.
     * @param entity The entity from which to dump.
     * @param energy The amount of energy to dump.
     */
    DumpCommand(Location entity, energy_type energy) : entity(entity), energy(energy) {}
};

/** Command to construct a drop zone. */
class ConstructCommand : public virtual Command, public Transactable<ConstructCommand> {
public:
    /** The entity to use to construct. */
    const Location entity;

    /**
     * Convert a ConstructCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Perform the construct on Map and Player.
     * @param map The Map.
     * @param player The Player.
     */
    void act(const Map &map, const Player &player) const;

    /**
     * Construct ConstructCommand from entity.
     * @param entity The entity to convert.
     */
    explicit ConstructCommand(Location entity) : entity(entity) {}
};

}

#endif // COMMAND_HPP
