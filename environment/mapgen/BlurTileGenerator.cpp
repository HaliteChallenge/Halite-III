#include "BlurTileGenerator.hpp"

namespace hlt {
namespace mapgen {
// Blur function takes in map, and a cell, and calculates a new production value for that cell
// based on some blurring with surrounding cells
// currently blurs entire map, not just a tile

long BlurTileGenerator::blur_function(dimension_type y_coord, dimension_type x_coord, const Map &map) const {
    // bring into local scope for shorter naming
    const auto BLUR_FACTOR = Constants::get().BLUR_FACTOR;
    // Weight of a neighbor's effect on a cell's production value is dependent on the number of neighbors being considered
    // Declare as local constant as rest of function code is also implicitly dependent on the number of neighbors
    const auto NUM_NEIGHBORS = 4;

    // Blur function looks only at immediate neighbors of a tile, wrapping around the edges of the tile as needed
    long left_coord = (x_coord - 1 + tile_width) % tile_width;
    long right_coord = (x_coord + 1) % tile_width;
    long above_coord = (y_coord - 1 + tile_height) % tile_height;
    long below_coord = (y_coord + 1) % tile_height;

    // In determining post blur production value, give current production value of cell weight BLUR_FACTOR
    // and production of neighbors weight (1 - BLUR_FACTOR)
    // This means that *each* neighbor's production gets weight (1 - BLUR_FACTOR) / 4
    // Truncate fractions via cast
    return static_cast<long>(map.grid[y_coord][x_coord]->production() * BLUR_FACTOR
                             + map.grid[y_coord][left_coord]->production() * (1 - BLUR_FACTOR) / NUM_NEIGHBORS
                             + map.grid[y_coord][right_coord]->production() * (1 - BLUR_FACTOR) / NUM_NEIGHBORS
                             + map.grid[above_coord][x_coord]->production() * (1 - BLUR_FACTOR) / NUM_NEIGHBORS
                             + map.grid[below_coord][x_coord]->production() * (1 - BLUR_FACTOR) / NUM_NEIGHBORS);
}

Map BlurTileGenerator::generate(std::list<Player> &players) {
    auto tile = make_map(tile_width, tile_height);
    const auto max = static_cast<double>(std::mt19937::max());

    // Fetch max and min values for square production and store for ease of use
    const auto MIN_CELL_PROD = Constants::get().MIN_CELL_PRODUCTION;
    const auto MAX_CELL_PROD = Constants::get().MAX_CELL_PRODUCTION;

    for (auto row = 0; row < tile_height; ++row) {
        for (auto col = 0; col < tile_width; ++col) {
            // randomly generate a production value using generator class' random number generator
            auto production = static_cast<energy_type>(rng() / max * (MAX_CELL_PROD - MIN_CELL_PROD) + MIN_CELL_PROD);
            tile.grid[row][col] = make_cell<NormalCell>(production);
        }
    }

    // For each cell, use the blur function to determine new production value for cell based on neighboring cell values
    // Blurring will create regression to mean, so additionally keep track of max value to later ensure full use of
    // production range
    long max_seen_prod = 0;
    for (auto row = 0; row < tile_height; ++row) {
        for (auto col = 0; col < tile_width; ++col) {
            // production is a private member, so create new cell with new production value
            const long post_blur_prod = blur_function(row, col, tile);
            if (post_blur_prod > max_seen_prod) max_seen_prod = post_blur_prod;
            tile.grid[row][col] = make_cell<NormalCell>(post_blur_prod);
        }
    }

    // Do a second pass over the tile to scale relative to post blur production, but to still get full range of
    // production values
    for (auto row = 0; row < tile_height; ++row) {
        for (auto col = 0; col < tile_width; ++col) {
            const long post_normalized_prod = tile.grid[row][col]->production() * MAX_CELL_PROD / max_seen_prod;
            tile.grid[row][col] = make_cell<NormalCell>(post_normalized_prod);
        }
    }

    const auto factory_pos_x = static_cast<dimension_type>((rng() / max * tile_width));
    const auto factory_pos_y = static_cast<dimension_type>((rng() / max * tile_height));

    // Use superclass function to copy the tile over the entire map, including placing all factories
    return tile_map(factory_pos_y, factory_pos_x, tile, players);
}

}
}
