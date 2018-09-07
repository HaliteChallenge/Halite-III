#pragma once

#include "direction.hpp"
#include "types.hpp"

#include <string>

namespace hlt {
    typedef std::string Command;

    namespace command {
        Command spawn_ship();
        Command transform_ship_into_dropoff_site(EntityId id);
        Command move(EntityId id, Direction direction);
    }
}
