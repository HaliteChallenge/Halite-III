#include "TileGenerator.hpp"

#include <cassert>

namespace hlt {
namespace mapgen {

Map TileGenerator::tile_map(const dimension_type factory_y, const dimension_type factory_x,
                            const Map &tile, std::list<Player> &players) {
    auto map = make_map(width, height);
    // Copy the tile over the map
    for (auto player_row = 0; player_row < num_tile_rows; ++player_row) {
        for (auto player_col = 0; player_col < num_tile_cols; ++player_col) {
            for (auto tile_row = 0; tile_row < tile_height; ++tile_row) {
                for (auto tile_col = 0; tile_col < tile_width; ++tile_col) {
                    map.grid[player_row * tile_height + tile_row][player_col * tile_width + tile_col] =
                            make_cell<NormalCell>(tile.grid[tile_row][tile_col]->production());
                }
            }
        }
    }

    // Place a factory for each player on the map at corresponding relative locations and update each
    // player to know their factory's location
    long player_idx = 0;
    for (auto &player : players) {
        const auto player_factory_x = (player_idx % num_tile_cols) * tile_width + factory_x;
        const auto player_factory_y = (player_idx / num_tile_cols) * tile_height + factory_y;
        map.grid[player_factory_y][player_factory_x] = make_cell<FactoryCell>();

        Player::Location factory_location{player_factory_x, player_factory_y};
        player.factory_location = factory_location;

        player_idx++;
    }

    return map;
}

TileGenerator::TileGenerator(const MapParameters &parameters) :
        Generator(parameters),
        num_players(parameters.num_players),
        width(parameters.width),
        height(parameters.height) {

    // Ensure that the map can be subdivided into partitions for a given number of players
    // ie: a 64x64 map cannot be (most basic definition of) symmetrical for 6 players
    // TODO: find a way to deal with cases when this is not true that's less disruptive than assertion errors - KB
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
    assert(tile_width * num_tile_cols == width && tile_height * num_tile_rows == height);

}

}
}
