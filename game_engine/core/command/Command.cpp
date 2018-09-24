#include <memory>

#include "BotCommunicationError.hpp"
#include "Command.hpp"
#include "CommandError.hpp"
#include "CommandTransaction.hpp"

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
void TransactableCommand<T>::add_to_transaction(Player &player, CommandTransaction &transaction) const {
    // Invoke overload resolution on CommandTransaction::add_command
    transaction.add_command(player, static_cast<const T &>(*this));
}

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
            command = std::make_unique<SpawnCommand>();
            break;
        }
        case Command::Name::Construct: {
            Entity::id_type entity;
            istream >> entity;
            command = std::make_unique<ConstructCommand>(entity);
            break;
        }
        default:
            throw BotCommunicationError(to_string(command_type), istream.tellg());
        }
    }
    return istream;
}

/**
 * Write a Command to bot serial format.
 * @param ostream The output stream.
 * @param[out] command The command to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, std::unique_ptr<Command> &command) {
    return ostream << command->to_bot_serial();
}

/**
 * Convert a MoveCommand to JSON format.
 * @param[out] json The JSON output.
 */
void MoveCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,      to_string(Name::Move)},
            {JSON_ENTITY_KEY,    entity},
            {JSON_DIRECTION_KEY, direction}};
}

/**
 * Convert to bot serial format.
 * @return The serialized command.
 */
std::string MoveCommand::to_bot_serial() const {
    return to_string(Name::Move) + " " + to_string(entity) + " " + to_string(static_cast<char>(direction));
}

/**
 * Convert a SpawnCommand to JSON format.
 * @param[out] json The JSON output.
 */
void SpawnCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,   to_string(Name::Spawn)}};
}

/**
 * Convert to bot serial format.
 * @return The serialized command.
 */
std::string SpawnCommand::to_bot_serial() const {
    return to_string(Name::Spawn);
}

/**
 * Convert a ConstructCommand to JSON format.
 * @param[out] json The JSON output.
 */
void ConstructCommand::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,   to_string(Name::Construct)},
            {JSON_ENTITY_KEY, entity}};
}

/**
 * Convert to bot serial format.
 * @return The serialized command.
 */
std::string ConstructCommand::to_bot_serial() const {
    return to_string(Name::Construct) + " " + to_string(entity);
}

}
