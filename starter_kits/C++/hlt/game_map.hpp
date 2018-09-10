#pragma once

#include "types.hpp"
#include "map_cell.hpp"

#include <vector>

namespace hlt {
    struct GameMap {
        int width;
        int height;
        std::vector<std::vector<MapCell>> cells;

        MapCell* at(const Position& position) {
            return &cells[position.y][position.x];
        }

        MapCell* at(const Entity& entity) {
            return at(entity.position);
        }

        MapCell* at(const Entity* entity) {
            return at(entity->position);
        }

        MapCell* at(const std::shared_ptr<Entity>& entity) {
            return at(entity->position);
        }

        int calculate_distance(const Position& source, const Position& target) {
            int dx = std::abs(source.x - target.x);
            int dy = std::abs(source.y - target.y);

            int wrapped_dx = std::min(dx, width - dx);
            int wrapped_dy = std::min(dy, height - dy);

            return wrapped_dx + wrapped_dy;
        }

        Position normalize(const Position& position) {
            return { position.x % width, position.y % height };
        }

        void _update();
        static std::unique_ptr<GameMap> _generate();
    };
}
