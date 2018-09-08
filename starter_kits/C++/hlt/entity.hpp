#pragma once

#include "types.hpp"
#include "position.hpp"

namespace hlt {
    struct Entity {
        PlayerId owner;
        EntityId id;
        Position position;

        Entity(PlayerId owner, EntityId id, int x, int y) :
            owner(owner),
            id(id),
            position(x, y)
        {}
    };
}
