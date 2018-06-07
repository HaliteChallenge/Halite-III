#include "Command.hpp"

namespace hlt {

void to_json(nlohmann::json &json, const Command &command) {
    json = {{"type",      static_cast<int>(command.type)},
            {"entity_id", command.entity_id},
            {"direction", static_cast<int>(command.direction)}};
}

void from_json(const nlohmann::json &json, Command &command) {
    command = {static_cast<Command::CommandType>(json.at("type").get<int>()),
               json.at("entity_id").get<Entity::EntityID>(),
               static_cast<Direction>(json.at("direction").get<int>())};
}

std::istream &operator>>(std::istream &is, Command &command) {
    char c;
    is >> c;
    switch (c) {
    case 'm':
        is >> command.entity_id >> command.direction;
    default:
        // TODO: implement
        break;
    }
    return is;
}

Command::Command(Command::CommandType type, Entity::EntityID entity_id, Direction direction) : type(type), entity_id(entity_id),
                                                                                               direction(direction) {}

}
