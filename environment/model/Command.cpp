#include <memory>

#include "Command.hpp"

namespace hlt {

void to_json(nlohmann::json &json, const Command &command) {
    command->to_json(json);
}

void from_json(const nlohmann::json &json, Command &command) {
    const auto &type = json.at("type").get<std::string>();
    if (type == "move") {
        command = std::make_unique<MoveCommand>(json);
    } else {
        // TODO: error case
    }
}

std::istream &operator>>(std::istream &istream, Command &command) {
    char command_type;
    istream >> command_type;
    std::string remainder;
    istream >> remainder;
    switch (command_type) {
    case 'm':
        command = std::make_unique<MoveCommand>(remainder);
        break;
    default:
        // TODO: error case
        break;
    }
    return istream;
}

void MoveCommand::to_json(nlohmann::json &json) const {
    json = {{"type",      "move"},
            {"entity_id", entity_id},
            {"direction", static_cast<int>(direction)}};
}

MoveCommand::MoveCommand(const nlohmann::json &json) :
        entity_id(json.at("entity_id").get<decltype(MoveCommand::entity_id)>()),
        direction(static_cast<Direction>(json.at("direction").get<int>())) {}

MoveCommand::MoveCommand(const std::string &bot_serial) {
    std::istringstream bot_stream(bot_serial);
    bot_stream >> entity_id;
    char direction_type;
    bot_stream >> direction_type;
    switch (direction_type) {
    case 'n':
        direction = Direction::North;
        break;
    case 's':
        direction = Direction::South;
        break;
    case 'e':
        direction = Direction::East;
        break;
    case 'w':
        direction = Direction::West;
        break;
    default:
        // TODO: error case
        break;
    }
}

void MoveCommand::act_on_map(Map &map) const {
    // TODO: implement
}

}
