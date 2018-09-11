#pragma once

#include "types.hpp"
#include "position.hpp"
#include "ship.hpp"
#include "dropoff.hpp"

namespace hlt {
    struct MapCell {
        Position position;
        Halite halite;
        std::shared_ptr<Ship> ship;
        std::shared_ptr<Entity> structure;

        MapCell(int x, int y, Halite halite) :
            position(x, y),
            halite(halite)
        {}

        bool is_empty() const {
            return !ship && !structure;
        }

        bool is_occupied() const {
            return static_cast<bool>(ship);
        }

        bool has_structure() const {
            return static_cast<bool>(structure);
        }

        void mark_unsafe(std::shared_ptr<Ship>& ship) {
            this->ship = ship;
        }
    };
}
