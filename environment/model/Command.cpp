#include <memory>

#include "BotCommandError.hpp"
#include "BotCommunicationError.hpp"
#include "Command.hpp"
#include "JsonError.hpp"

#include "util.hpp"

/** The JSON key for command type. */
constexpr auto JSON_TYPE_KEY = "type";
/** The JSON key for entity X location. */
constexpr auto JSON_ENTITY_X_KEY = "entity_x";
/** The JSON key for entity Y location. */
constexpr auto JSON_ENTITY_Y_KEY = "entity_y";
/** The JSON key for direction. */
constexpr auto JSON_DIRECTION_KEY = "direction";
/** The JSON key for energy. */
constexpr auto JSON_ENERGY_KEY = "energy";
/** The JSON key for factory. */
constexpr auto JSON_FACTORY_KEY = "factory";

namespace hlt {

/**
 * Convert a Command to JSON format.
 * @param[out] json The output JSON.
 * @param command The command to convert.
 */
void to_json(nlohmann::json &json, const Command &command) { command->to_json(json); }

/**
 * Read a Command from bot serial format.
 * @param istream The input stream.
 * @param[out] command The command to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, Command &command) {
    // Read one character corresponding to the type, and dispatch the remainder based on its value.
    char command_type;
    if (istream >> command_type) {
        switch (command_type) {
        case MoveCommand::COMMAND_TYPE_SHORT: {
            dimension_type entity_x, entity_y;
            Direction direction;
            istream >> entity_x >> entity_y >> direction;
            command = std::make_unique<MoveCommand>(entity_x, entity_y, direction);
            break;
        }
        case BuyCommand::COMMAND_TYPE_SHORT: {
            energy_type energy;
            Location location;
            istream >> energy >> location;
            command = std::make_unique<BuyCommand>(energy, location);
            break;
        }
        case SellCommand::COMMAND_TYPE_SHORT: {
            Location location;
            istream >> location;
            command = std::make_unique<SellCommand>(location);
            break;
        }
        case SpawnCommand::COMMAND_TYPE_SHORT: {
            energy_type energy;
            istream >> energy;
            command = std::make_unique<SpawnCommand>(energy);
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
    json = {{JSON_TYPE_KEY,      MoveCommand::COMMAND_TYPE_NAME},
            {JSON_ENTITY_X_KEY,  entity_x},
            {JSON_ENTITY_Y_KEY,  entity_y},
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
 * Cause the move to act on the Map.
 * @param transaction The command transaction to act on.
 */
void MoveCommand::act_on_map(CommandTransaction &transaction) const {
    Location location{entity_x, entity_y};
    auto &player = transaction.player;
    if (player.find_entity(location) == nullptr) {
        throw BotCommandError("Attempt by player " + std::to_string(player.player_id) + " to move unowned entity");
    } else {
        auto destination = location;
        // Move the location by the specified direction to get the destination
        transaction.map.move_location(destination, direction);
        transaction.move_entity(location, destination);
    }
}

/**
 * Convert a BuyCommand to JSON format.
 * @param[out] json The JSON output.
 */
void BuyCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,     BuyCommand::COMMAND_TYPE_NAME},
            {JSON_ENERGY_KEY,   price},
            {JSON_FACTORY_KEY,  factory}};
}

/**
 * Convert a SellCommand to JSON format.
 * @param[out] json The JSON output.
 */
void SellCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,     SellCommand::COMMAND_TYPE_NAME},
            {JSON_FACTORY_KEY,  factory}};
}

void SpawnCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,    SpawnCommand::COMMAND_TYPE_NAME},
            {JSON_ENERGY_KEY,  energy}};
}

void SpawnCommand::act_on_map(CommandTransaction &transaction) const {
    transaction.spawn_entity(transaction.player.factories.front(), energy);
}

}
