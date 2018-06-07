#ifndef MOVE_H
#define MOVE_H

#include <deque>

#include "Entity.hpp"

#include "util/json.hpp"

namespace hlt {

/** Represents a command that may be issued to an entity. */
struct Command {
    /** The different types of possible commands issued by the user. */
    enum class CommandType {
        Noop = 0, /**< No command was issued. */
        Move = 1,
        Error = 2, /**< The move is invalid. */
    };

    CommandType type;
    Entity::EntityID entity_id;
    Direction direction;

    friend void to_json(nlohmann::json &json, const Command &command);

    friend void from_json(const nlohmann::json &json, Command &command);

    friend std::istream &operator>>(std::istream &is, Command &command);

    Command(CommandType type, Entity::EntityID entity_id, Direction direction);
};

}

#endif // MOVE_H
