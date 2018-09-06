#pragma once

#include "entity.hpp"
#include "command.hpp"

namespace hlt {
    struct Shipyard : Entity {
        Command spawn() {
            return hlt::command::spawn_ship();
        }
    };
}
