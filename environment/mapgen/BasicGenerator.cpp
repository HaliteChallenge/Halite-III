#include "BasicGenerator.hpp"
#include "Map.hpp"

namespace hlt {
namespace mapgen {

std::string BasicGenerator::name() const {
    return "Basic Generator";
}

hlt::Map BasicGenerator::generate(std::list<Player> &players) {
    auto map = Map(width, height);
    for (auto &row : map.grid) {
        for (auto &cell : row) {
            cell.production = 10;
            cell.type = Cell::CellType::Normal;
        }
    }
    unsigned long player_gap = width / players.size();
    for (auto &player : players) {
        map.grid[height / 2][player.player_id * player_gap] = player.factory_cell;
    }
    return map;
}

BasicGenerator::BasicGenerator(const MapParameters &parameters) : Generator(parameters), width(parameters.width),
                                                                  height(parameters.height),
                                                                  num_players(parameters.num_players) {}

}
}
