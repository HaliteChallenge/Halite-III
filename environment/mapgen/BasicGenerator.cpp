#include "BasicGenerator.hpp"

/** Base production of the map cells. */
constexpr auto PRODUCTION = 10;

namespace hlt {
namespace mapgen {

/**
 * Generate a map based on a list of players.
 * @param players The players on the map.
 */
hlt::Map BasicGenerator::generate(std::unordered_map<id_type, Player> &players) {
    (void) players;
    auto map = make_map(width, height);
    // Each cell in the map is simply NormalCell with a fixed production, for now.
    for (auto y = 0; y < map.height; y++) {
        for (auto x = 0; y < map.width; x++) {
            map.at(x, y) = make_cell<NormalCell>(PRODUCTION);
        }
    }
    return map;
}

}
}
