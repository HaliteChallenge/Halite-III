#include <memory>

#include "BotCommandError.hpp"
#include "BotCommunicationError.hpp"
#include "Command.hpp"
#include "JsonError.hpp"

#include "nlohmann/json.hpp"

/** The JSON key for command type. */
constexpr auto JSON_TYPE_KEY = "type";
/** The JSON key for entity X location. */
constexpr auto JSON_ENTITY_X_KEY = "entity_x";
/** The JSON key for entity Y location. */
constexpr auto JSON_ENTITY_Y_KEY = "entity_y";
/** The JSON key for direction. */
constexpr auto JSON_DIRECTION_KEY = "direction";

namespace hlt {

/**
 * Convert a Command to JSON format.
 * @param[out] json The output JSON.
 * @param command The command to convert.
 */
void to_json(nlohmann::json &json, const Command &command) { command->to_json(json); }

/**
 * Convert an encoded Command from JSON format.
 * @param json The JSON.
 * @param[out] command The converted command.
 */
void from_json(const nlohmann::json &json, Command &command) {
    // The type field determines the Command subclass that will be instantiated.
    const std::string &type = json.at(JSON_TYPE_KEY);
    if (type == MoveCommand::COMMAND_TYPE_NAME) {
        command = std::make_unique<MoveCommand>(json);
    } else {
        throw JsonError(json);
    }
}

/**
 * Read a Command from bot serial format.
 * @param istream The input stream.
 * @param[out] command The command to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, Command &command) {
    // Read one character corresponding to the type, and dispatch the remainder based on its value.
    std::string line;
    std::getline(istream, line);
    std::istringstream stream(line);
    char command_type;
    stream >> command_type;
    std::string remainder;
    std::getline(stream, remainder);
    try {
        switch (command_type) {
        case MoveCommand::COMMAND_TYPE_SHORT:
            command = std::make_unique<MoveCommand>(remainder);
            break;
        default:
            throw BotCommunicationError(line);
        }
    } catch (BotCommunicationError &error) {
        throw BotCommunicationError(line);
    }
    return istream;
}

constexpr char const *MoveCommand::COMMAND_TYPE_NAME;

/**
 * Convert a MoveCommand to JSON format.
 * @param[out] json The JSON output.
 */
void MoveCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,      MoveCommand::COMMAND_TYPE_NAME},
            {JSON_ENTITY_X_KEY, entity_x},
            {JSON_ENTITY_Y_KEY, entity_y},
            {JSON_DIRECTION_KEY, direction}};
}

/**
 * Create MoveCommand from JSON.
 * @param json The JSON.
 */
MoveCommand::MoveCommand(const nlohmann::json &json) :
        entity_x(json.at(JSON_ENTITY_X_KEY)),
        entity_y(json.at(JSON_ENTITY_Y_KEY)),
        direction(json.at(JSON_DIRECTION_KEY)) {}

/**
 * Create MoveCommand from bot serial format.
 * @param bot_serial The bot serial format input.
 */
MoveCommand::MoveCommand(const std::string &bot_serial) {
    // Read the entity ID and the direction.
    std::istringstream bot_stream(bot_serial);
    try {
        bot_stream >> entity_x >> entity_y >> direction;
    } catch (std::istream::failure &failure) {
        throw BotCommunicationError(bot_serial);
    }
}

/**
 * Cause the move to act on the Map.
 * @param map The Map to act on.
 * @param player The player who is issuing the command.
 */
void MoveCommand::act_on_map(Map &map, Player &player) const {
    auto location = std::make_pair(entity_x, entity_y);
    auto player_entity_iterator = player.entities.find(location);
    if (player_entity_iterator == player.entities.end()) {
        throw BotCommandError("Attempt by player " + std::to_string(player.player_id) + " to move unowned entity");
    } else {
        //std::shared_ptr<Entity> entity = player_entity_iterator->second;
        // Move, because the player will then remove it
        auto entity = std::move(player_entity_iterator->second);
        // Remove the old entity
        player.remove_entity(location);
        map.at(location)->remove_entity(player);

        map.move_location(location, direction);

        // Add the new entity
        player.add_entity(location, entity);
        map.at(location)->add_entity(player, entity);
    }
}

}
