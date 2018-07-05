#include "BasicGenerator.hpp"

/** Base production of the map cells. */
constexpr auto PRODUCTION = 10;

namespace hlt {
namespace mapgen {

void BasicGenerator::generate(Map &map, std::vector<Location> &factories) {
    // Each cell in the map is simply NormalCell with a fixed production, for now.
    auto players = num_players;
    for (dimension_type y = 0; y < map.height; y++) {
        for (dimension_type x = 0; x < map.width; x++) {
            if (players-- > 0) {
                map.at(x, y) = make_cell<FactoryCell>();
                factories.emplace_back(x, y);
            } else {
                map.at(x, y) = make_cell<NormalCell>(PRODUCTION);
            }
        }
    }
}

}
}
