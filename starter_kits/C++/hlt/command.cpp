#include "command.hpp"

#include <iostream>

constexpr char GENERATE= 'g';
constexpr char CONSTRUCT[] = "c ";
constexpr char MOVE[] = "m ";

hlt::Command hlt::command::spawn_ship() {
    return std::string(1, GENERATE);
}

hlt::Command hlt::command::transform_ship_into_dropoff_site(EntityId id) {
    return CONSTRUCT + std::to_string(id);
}

hlt::Command hlt::command::move(EntityId id, hlt::Direction direction) {
    return MOVE + std::to_string(id) + ' ' + static_cast<char>(direction);
}
