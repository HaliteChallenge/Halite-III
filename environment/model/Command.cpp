#include <memory>

#include "Command.hpp"

/** The JSON key for command type. */
constexpr auto JSON_TYPE_KEY = "type";
/** The JSON key for entity ID. */
constexpr auto JSON_ENTITY_ID_KEY = "entity_id";
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
    const auto &type = json.at(JSON_TYPE_KEY).get<std::string>();
    if (type == MoveCommand::COMMAND_TYPE_NAME) {
        command = std::make_unique<MoveCommand>(json);
    } else {
        // TODO: error case
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
    char command_type;
    istream >> command_type;
    std::string remainder;
    istream >> remainder;
    switch (command_type) {
    case MoveCommand::COMMAND_TYPE_SHORT:
        command = std::make_unique<MoveCommand>(remainder);
        break;
    default:
        // TODO: error case
        break;
    }
    return istream;
}

/**
 * Convert a MoveCommand to JSON format.
 * @param[out] json The JSON output.
 */
void MoveCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,      MoveCommand::COMMAND_TYPE_NAME},
            {JSON_ENTITY_ID_KEY, entity_id},
            {JSON_DIRECTION_KEY, static_cast<int>(direction)}};
}

/**
 * Create MoveCommand from JSON.
 * @param json The JSON.
 */
MoveCommand::MoveCommand(const nlohmann::json &json) :
        entity_id(json.at(JSON_ENTITY_ID_KEY).get<decltype(MoveCommand::entity_id)>()),
        direction(static_cast<Direction>(json.at(JSON_DIRECTION_KEY).get<int>())) {}

/**
 * Create MoveCommand from bot serial format.
 * @param bot_serial The bot serial format input.
 */
MoveCommand::MoveCommand(const std::string &bot_serial) {
    // Read the entity ID and the direction.
    std::istringstream bot_stream(bot_serial);
    bot_stream >> entity_id >> direction;
}

/**
 * Cause the MoveCommand to act on a Map.
 * @param map The Map to act on.
 */
void MoveCommand::act_on_map(Map &map) const {
    // TODO: implement
}

}
