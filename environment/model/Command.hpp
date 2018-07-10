#ifndef MOVE_H
#define MOVE_H

#include <deque>

#include "CommandTransaction.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "Location.hpp"
#include "Map.hpp"

#include "nlohmann/json_fwd.hpp"

namespace hlt {

class BaseCommand;

/** Container type for command, exposed to outside users. */
using Command = std::shared_ptr<BaseCommand>;

/**
 * Convert a Command to JSON format.
 * @param[out] json The output JSON.
 * @param command The command to convert.
 */
void to_json(nlohmann::json &json, const Command &cell);

/**
 * Read a Command from bot serial format.
 * @param istream The input stream.
 * @param[out] command The command to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, Command &command);

/** Abstract type of commands issued by the user. */
class BaseCommand {
public:
    /**
     * JSON encoding function, dispatched on command subtypes.
     * @param[out] json The JSON output.
     */
    virtual void to_json(nlohmann::json &json) const = 0;

    /**
     * If the command has an action on the Map, cause it to occur.
     * @param transaction The command transaction to act on.
     */
    virtual void act_on_map(CommandTransaction &transaction) const = 0;

    virtual ~BaseCommand() = default;
};

/** Command for moving an entity in a direction. */
class MoveCommand : public BaseCommand {
    dimension_type entity_x; /**< The ID of the entity to move. */
    dimension_type entity_y; /**< The ID of the entity to move. */
    Direction direction;     /**< The direction in which to move the entity. */

public:
    /** The name of the move command. */
    static constexpr auto COMMAND_TYPE_NAME = "move";

    /** The short name of the move command. */
    static constexpr auto COMMAND_TYPE_SHORT = 'm';

    /**
     * Convert a MoveCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Cause the move to act on the Map.
     * @param transaction The command transaction to act on.
     */
    void act_on_map(CommandTransaction &transaction) const override;

    /**
     * Create MoveCommand from entity ID and direction.
     * @param entity_x The x coordinate of the entity
     * @param entity_y The y coordinate of the entity
     * @param direction The direction.
     */
    MoveCommand(dimension_type entity_x, dimension_type entity_y, Direction direction)
            : entity_x(entity_x), entity_y(entity_y), direction(direction) {}

    /**
     * Create MoveCommand from JSON.
     * @param json The JSON.
     */
    explicit MoveCommand(const nlohmann::json &json);
};

/** Command for an attempt to buy a factory. */
class BuyCommand : public BaseCommand {
    energy_type price;  /**< The price. */
    Location factory;   /**< The factory being bought. */

public:
    /** The name of the buy command. */
    static constexpr auto COMMAND_TYPE_NAME = "buy";

    /** The short name of the buy command. */
    static constexpr auto COMMAND_TYPE_SHORT = 'b';

    /**
     * Convert a BuyCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /** No effect on the map. */
    void act_on_map(CommandTransaction &) const override {}

    /**
     * Construct BuyCommand from price and factory.
     * @param price The price.
     * @param factory The factory.
     */
    BuyCommand(energy_type price, const Location &factory) : price(price), factory(factory) {}
};

/** Command for selling a factory. */
class SellCommand : public BaseCommand {
    Location factory;   /**< The factory being sold. */

public:
    /** The name of the sell command. */
    static constexpr auto COMMAND_TYPE_NAME = "sell";

    /** The short name of the sell command. */
    static constexpr auto COMMAND_TYPE_SHORT = 's';

    /**
     * Convert a SellCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /** No effect on the map. */
    void act_on_map(CommandTransaction &) const override {}

    /**
    * Construct SellCommand from factory.
    * @param factory The factory.
    */
    explicit SellCommand(const Location &factory) : factory(factory) {}
};

/** Command for moving an entity in a direction. */
class SpawnCommand : public BaseCommand {
    energy_type energy;     /**< The amount of energy to give the new entity. */

public:
    /** The name of the spawn command. */
    static constexpr auto COMMAND_TYPE_NAME = "spawn";

    /** The short name of the spawn command. */
    static constexpr auto COMMAND_TYPE_SHORT = 'g';

    /**
     * Convert a SpawnCommand to JSON format.
     * @param[out] json The JSON output.
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Cause the move to act on the Map.
     * @param transaction The command transaction to act on.
     */
    void act_on_map(CommandTransaction &transaction) const override;

    /**
     * Create SpawnCommand
     * @param energy The energy to use.
     */
    SpawnCommand(energy_type energy)
            : energy(energy) {}

    /**
     * Create MoveCommand from JSON.
     * @param json The JSON.
     */
    explicit SpawnCommand(const nlohmann::json &json);
};

}

#endif // MOVE_H
