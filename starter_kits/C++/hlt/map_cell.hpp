#pragma once

#include "types.hpp"
#include "position.hpp"
#include "ship.hpp"
#include "dropoff.hpp"

namespace hlt {
    struct MapCell {
        Position position;
        Halite halite_amount;
        Ship* ship;
        Entity* structure;

        bool is_empty() const {
            return ship == nullptr && structure == nullptr;
        }

        bool is_occupied() const {
            return ship != nullptr;
        }

        bool has_structure() const {
            return structure != nullptr;
        }

        void mark_unsafe(Ship* ship) {
            this->ship = ship;
        }
    };
}
