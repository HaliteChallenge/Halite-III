#include "BasicGenerator.hpp"
#include "Map.hpp"

namespace hlt {
namespace mapgen {

hlt::Map BasicGenerator::generate(std::list<Player> &players) const {
    auto map = Map(width, height);
    for (auto &row : map.grid) {
        for (auto &cell : row) {
            cell = std::make_unique<NormalCell>(10);
        }
    }
    return map;
}

}
}
