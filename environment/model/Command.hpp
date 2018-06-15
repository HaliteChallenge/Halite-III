#ifndef MOVE_H
#define MOVE_H

#include <deque>

#include "Entity.hpp"
#include "Player.hpp"

#include "nlohmann/json_fwd.hpp"

namespace hlt {

class BaseCommand;

/** Container type for command, exposed to outside users. */
using Command = std::unique_ptr<BaseCommand>;

/**
 * Convert a Command to JSON format.
 * @param[out] json The output JSON.
 * @param command The command to convert.
 */
void to_json(nlohmann::json &json, const Command &cell);

/**
 * Convert an encoded Command from JSON format.
 * @param json The JSON.
 * @param[out] command The converted command.
 */
void from_json(const nlohmann::json &json, Command &cell);

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
     * @param map The Map to act on.
     * @param player The player who is issuing the command.
     */
    virtual void act_on_map(Map &map, Player &player) const = 0;

    virtual ~BaseCommand() = default;
};

/** Command for moving an entity in a direction. */
class MoveCommand : public BaseCommand {
    Entity::id_type entity_id;  /**< The ID of the entity to move. */
    Direction direction;         /**< The direction in which to move the entity. */

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
     * @param map The Map to act on.
     * @param player The player who is issuing the command.
     */
    void act_on_map(Map &map, Player &player) const override;

    /**
     * Create MoveCommand from entity ID and direction.
     * @param entity_id The entity ID.
     * @param direction The direction.
     */
    MoveCommand(Entity::id_type entity_id, Direction direction)
            : entity_id(entity_id), direction(direction) {}

    /**
     * Create MoveCommand from JSON.
     * @param json The JSON.
     */
    explicit MoveCommand(const nlohmann::json &json);

    /**
     * Create MoveCommand from bot serial format.
     * @param bot_serial The bot serial format input.
     */
    explicit MoveCommand(const std::string &bot_serial);
};

}

#endif // MOVE_H
