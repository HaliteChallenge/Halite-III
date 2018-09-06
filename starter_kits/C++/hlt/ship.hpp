#pragma once

#include "entity.hpp"
#include "constants.hpp"
#include "command.hpp"

namespace hlt {
    struct Ship : Entity {
        Halite halite_amount;

        bool is_full() const {
            return halite_amount >= constants::MAX_HALITE;
        }

        Command make_dropoff() const {
            return hlt::command::transform_ship_into_dropoff_site(id);
        }

        Command move(Direction direction) const {
            return hlt::command::move(id, direction);
        }

        Command stay_still() const {
            return hlt::command::move(id, Direction::STILL);
        }
    };
}
