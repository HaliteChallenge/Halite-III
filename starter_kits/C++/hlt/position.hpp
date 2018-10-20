#pragma once

#include "types.hpp"
#include "direction.hpp"

#include <iostream>

namespace hlt {
    struct Position {
        int x;
        int y;

        Position() : x(0), y(0) {}
        Position(int x, int y) : x(x), y(y) {}

        bool operator==(const Position& other) const { return x == other.x && y == other.y; }
        bool operator!=(const Position& other) const { return x != other.x || y != other.y; }

        // strict weak ordering, useful for non-hash-based maps
        bool operator<(const Position &other) const {
            if (y != other.y) {
                return y < other.y;
            }
            return x < other.x;
        }

        std::string to_string() const {
            return std::to_string(x) + ':' + std::to_string(y);
        }

        Position directional_offset(Direction d) const {
            auto dx = 0;
            auto dy = 0;
            switch (d) {
                case Direction::NORTH:
                    dy = -1;
                    break;
                case Direction::SOUTH:
                    dy = 1;
                    break;
                case Direction::EAST:
                    dx = 1;
                    break;
                case Direction::WEST:
                    dx = -1;
                    break;
                case Direction::STILL:
                    // No move
                    break;
                default:
                    log::log(std::string("Error: directional_offset: unknown direction ") + static_cast<char>(d));
                    exit(1);
            }
            return Position{x + dx, y + dy};
        }

        std::array<Position, 4> get_surrounding_cardinals() {
            return {{
                directional_offset(Direction::NORTH), directional_offset(Direction::SOUTH),
                directional_offset(Direction::EAST), directional_offset(Direction::WEST)
            }};
        }
    };

    static std::ostream& operator<<(std::ostream& out, const Position& position) {
        out << position.x << ' ' << position.y;
        return out;
    }
    static std::istream& operator>>(std::istream& in, Position& position) {
        in >> position.x >> position.y;
        return in;
    }
}

namespace std {
    template <>
    struct hash<hlt::Position> {
        std::size_t operator()(const hlt::Position& position) const {
            return ((position.x+position.y) * (position.x+position.y+1) / 2) + position.y;
        }
    };
}
