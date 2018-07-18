#include <memory>

#include "BotCommandError.hpp"
#include "BotCommunicationError.hpp"
#include "Command.hpp"

/** The JSON key for command type. */
constexpr auto JSON_TYPE_KEY = "type";
/** The JSON key for entity id. */
constexpr auto JSON_ENTITY_KEY = "id";
/** The JSON key for direction. */
constexpr auto JSON_DIRECTION_KEY = "direction";
/** The JSON key for energy. */
constexpr auto JSON_ENERGY_KEY = "energy";

namespace hlt {

template<class T>
TransactableCommand<T>::~TransactableCommand() = default;

/**
 * Convert a Command to JSON format.
 * @param[out] json The output JSON.
 * @param command The command to convert.
 */
void to_json(nlohmann::json &json, const Command &command) { command.to_json(json); }

/**
 * Convert a Command ptr to JSON format.
 * @param[out] json The output JSON.
 * @param command The command to convert.
 */

void to_json(nlohmann::json &json, const std::unique_ptr<Command> &command) { command->to_json(json); }

/**
 * Read a Command from bot serial format.
 * @param istream The input stream.
 * @param[out] command The command to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, std::unique_ptr<Command> &command) {
    // Read one character corresponding to the type, and dispatch the remainder based on its value.
    char command_type;
    if (istream >> command_type) {
        switch (command_type) {
        case Command::Name::Move: {
            Entity::id_type entity;
            Direction direction;
            istream >> entity >> direction;
            command = std::make_unique<MoveCommand>(entity, direction);
            break;
        }
        case Command::Name::Spawn: {
            energy_type energy;
            istream >> energy;
            command = std::make_unique<SpawnCommand>(energy);
            break;
        }
        case Command::Name::Dump: {
            Entity::id_type entity;
            energy_type energy{};
            istream >> entity >> energy;
            command = std::make_unique<DumpCommand>(entity, energy);
            break;
        }
        case Command::Name::Construct: {
            Entity::id_type entity;
            istream >> entity;
            command = std::make_unique<ConstructCommand>(entity);
            break;
        }
        default:
            throw BotCommunicationError(to_string(command_type));
        }
    }
    return istream;
}

/**
 * Convert a MoveCommand to JSON format.
 * @param[out] json The JSON output.
 */
void MoveCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,      Name::Move},
            {JSON_ENTITY_KEY,    entity},
            {JSON_DIRECTION_KEY, direction}};
}

/**
 * Convert a SpawnCommand to JSON format.
 * @param[out] json The JSON output.
 */
void SpawnCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,   Name::Spawn},
            {JSON_ENERGY_KEY, energy}};
}

/**
 * Convert a DumpCommand to JSON format.
 * @param[out] json The JSON output.
 */
void DumpCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,   Name::Dump},
            {JSON_ENTITY_KEY, entity},
            {JSON_ENERGY_KEY, energy}};
}

/**
 * Convert a ConstructCommand to JSON format.
 * @param[out] json The JSON output.
 */
void ConstructCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,   Name::Construct},
            {JSON_ENTITY_KEY, entity}};
}

}
