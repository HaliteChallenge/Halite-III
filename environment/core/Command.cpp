#include <memory>

#include "BotCommandError.hpp"
#include "BotCommunicationError.hpp"
#include "Command.hpp"
#include "JsonError.hpp"

#include "util.hpp"

/** The JSON key for command type. */
constexpr auto JSON_TYPE_KEY = "type";
/** The JSON key for entity location. */
constexpr auto JSON_ENTITY_KEY = "entity";
/** The JSON key for direction. */
constexpr auto JSON_DIRECTION_KEY = "direction";
/** The JSON key for energy. */
constexpr auto JSON_ENERGY_KEY = "energy";

namespace hlt {

/**
 * Convert a Command to JSON format.
 * @param[out] json The output JSON.
 * @param command The command to convert.
 */
void to_json(nlohmann::json &json, const Command &command) { command.to_json(json); }

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
            Location entity;
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
            Location entity;
            energy_type energy;
            istream >> entity >> energy;
            command = std::make_unique<DumpCommand>(entity, energy);
            break;
        }
        case Command::Name::Construct: {
            Location entity;
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

void MoveCommand::add_to_transaction(CommandTransaction &transaction) const {
    // TODO: implement
    (void)transaction;
}

void MoveCommand::act(Map &map, Player &player) const {
    // TODO: implement
    (void)map;
    (void)player;
}

/**
 * Convert a SpawnCommand to JSON format.
 * @param[out] json The JSON output.
 */
void SpawnCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,   Name::Spawn},
            {JSON_ENERGY_KEY, energy}};
}

void SpawnCommand::add_to_transaction(CommandTransaction &transaction) const {
    // TODO: implement
    (void)transaction;
}

void SpawnCommand::act(Map &map, Player &player) const {
    // TODO: implement
    (void)map;
    (void)player;
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

void DumpCommand::add_to_transaction(CommandTransaction &transaction) const {
    // TODO: implement
    (void)transaction;
}

/**
 * Convert a ConstructCommand to JSON format.
 * @param[out] json The JSON output.
 */
void ConstructCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,   Name::Construct},
            {JSON_ENTITY_KEY, entity}};
}

void ConstructCommand::add_to_transaction(CommandTransaction &transaction) const {
    // TODO: implement
    (void)transaction;
}

}
