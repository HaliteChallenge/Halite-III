#pragma once

#include "types.hpp"
#include "position.hpp"

namespace hlt {
    struct Entity {
        PlayerId owner;
        EntityId id;
        Position position;
    };
}
