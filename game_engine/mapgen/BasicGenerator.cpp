#include "BasicGenerator.hpp"

/** Base energy of the map cells. */
constexpr auto ENERGY = 10;

namespace hlt {
namespace mapgen {

void BasicGenerator::generate(Map &map) {
    // Each cell in the map is simply normal with a fixed production.
    auto players = num_players;
    for (dimension_type y = 0; y < map.height; y++) {
        for (dimension_type x = 0; x < map.width; x++) {
            if (players-- > 0) {
                map.factories.emplace_back(x, y);
            }
            map.at(x, y).energy = ENERGY;
        }
    }
}

}
}
