#pragma once

#include "log.hpp"

#include <ostream>
#include <array>

namespace hlt {
    enum class Direction : char {
        NORTH = 'n',
        EAST = 'e',
        SOUTH = 's',
        WEST = 'w',
        STILL = 'o',
    };

    static const std::array<Direction, 4> ALL_CARDINALS = {
        { Direction::NORTH, Direction::SOUTH, Direction::EAST, Direction::WEST }
    };

    static Direction invert_direction(Direction direction) {
        switch (direction) {
            case Direction::NORTH:
                return Direction::SOUTH;
            case Direction::SOUTH:
                return Direction::NORTH;
            case Direction::EAST:
                return Direction::WEST;
            case Direction::WEST:
                return Direction::EAST;
            case Direction::STILL:
                return Direction::STILL;
            default:
                log::log(std::string("Error: invert_direction: unknown direction ") + static_cast<char>(direction));
                exit(1);
        }
    }

    static std::ostream& operator<<(std::ostream& out, const Direction& direction) {
        return out << static_cast<char>(direction);
    }
}
