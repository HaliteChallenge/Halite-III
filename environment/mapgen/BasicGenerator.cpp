#include "BasicGenerator.hpp"

/** Base production of the map cells. */
constexpr auto PRODUCTION = 10;

namespace hlt {
namespace mapgen {

/**
 * Generate a map based on a list of players.
 * @param players The players on the map.
 */
hlt::Map BasicGenerator::generate(std::list<Player> &players)  {
    (void) players;
    auto map = Map(width, height);
    // Each cell in the map is simply NormalCell with a fixed production, for now.
    for (auto &row : map.grid) {
        for (auto &cell : row) {
            cell = std::make_unique<NormalCell>(PRODUCTION);
        }
    }
    return map;
}

}
}
