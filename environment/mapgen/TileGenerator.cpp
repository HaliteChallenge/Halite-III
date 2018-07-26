#include "TileGenerator.hpp"
#include "Logging.hpp"

namespace hlt {
namespace mapgen {

void TileGenerator::tile_map(Map &map, const dimension_type factory_y, const dimension_type factory_x,
                             const Map &tile) {
    // Copy the tile over the map
    for (dimension_type player_row = 0; player_row < num_tile_rows; ++player_row) {
        for (dimension_type player_col = 0; player_col < num_tile_cols; ++player_col) {
            for (dimension_type tile_row = 0; tile_row < tile_height; ++tile_row) {
                for (dimension_type tile_col = 0; tile_col < tile_width; ++tile_col) {
                    map.at(player_col * tile_width + tile_col, player_row * tile_height + tile_row).energy =
                            tile.at(tile_col, tile_row).energy;
                }
            }
        }
    }

    // Place a factory for each player on the map at corresponding relative locations
    for (unsigned long player_idx = 0; player_idx < num_players; player_idx++) {
        const dimension_type player_factory_x = (player_idx % num_tile_cols) * tile_width + factory_x;
        const dimension_type player_factory_y = (player_idx / num_tile_cols) * tile_height + factory_y;
        map.at(player_factory_x, player_factory_y).energy = 0;

        map.factories.emplace_back(player_factory_x, player_factory_y);
    }
}

TileGenerator::TileGenerator(const MapParameters &parameters) :
        Generator(parameters),
        width(parameters.width),
        height(parameters.height),
        num_players(parameters.num_players) {

    // Ensure that the map can be subdivided into partitions for a given number of players
    // ie: a 64x64 map cannot be (most basic definition of) symmetrical for 6 players
    if ((width * height) % num_players != 0) {
        std::ostringstream stream;
        stream << "The map size given must be evenly divisible by the number of players. A map size of " << width << " x " << height;
        stream << " with area of " << width * height << " violates that for " << num_players << " players.";
        Logging::log(stream.str(), Logging::Level::Error);
    }
    assert((width * height) % num_players == 0);

    // We want our map to be as square-like as possible, so to find the size of a tile, first determine
    // how many tiles we will have
    num_tile_rows = static_cast<dimension_type>(sqrt(num_players));
    while (num_players % num_tile_rows != 0) {
        num_tile_rows--;
    }
    num_tile_cols = num_players / num_tile_rows;

    // next, use number of rows and cols of player start tiles to determine height and width of a single tile
    tile_width = width / num_tile_cols;
    tile_height = height / num_tile_rows;

    // Ensure these tiles then make up the whole map
    if (!(tile_width * num_tile_cols == width && tile_height * num_tile_rows == height)) {
        std::ostringstream stream;
        stream << "Map must be able to be created by copying one tile over the whole map. For the given number of players we have a ";
        stream << tile_width << " x " << tile_height << " tile, which cannot perfectly tile the given map size, "<<  width << "x" << height;
        stream << ". Try using map dimensions that are powers of 2.";
        Logging::log(stream.str(), Logging::Level::Error);
    }
    assert(tile_width * num_tile_cols == width && tile_height * num_tile_rows == height);

}

}
}
