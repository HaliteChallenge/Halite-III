#pragma once

#include "types.hpp"
#include "shipyard.hpp"
#include "ship.hpp"
#include "dropoff.hpp"

#include <memory>
#include <unordered_map>

namespace hlt {
    struct Player {
        PlayerId id;
        Shipyard shipyard;
        Halite halite_amount;
        std::unordered_map<EntityId, Ship> ships;
        std::unordered_map<EntityId, Dropoff> dropoffs;

        void _update(int num_ships, int num_dropoffs, Halite halite_amount);
        static Player* _generate();
    };
}
