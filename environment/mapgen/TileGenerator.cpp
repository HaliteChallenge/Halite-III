#include "TileGenerator.hpp"
#include "Map.hpp"
#include "MapConstants.hpp"

namespace hlt {
    namespace mapgen {
        // Blur function takes in map, and a cell, and calculates a new production value for that cell
        // based on some blurring with surrounding cells
        // currently blurs entire map, not just a tile
        long TileGenerator::blur_function(const long x_coord, const long y_coord, const hlt::Map &map) const {
            // rename to use more easily
            double BLUR_FACTOR = hlt::MapConstants::get().BLUR_FACTOR;
            long left_coord = (x_coord - 1 + width) % width;
            long right_coord = (x_coord + 1) % width;
            long above_coord = (y_coord - 1 + height) % height;
            long below_coord = (y_coord + 1) % height;

            /** In determining post blur production value, give current production value of cell weight BLUR_FACTOR
             * and production of neighbors weight (1 - BLUR_FACTOR)
             * This means that *each* neighbor's production gets weight (1 - BLUR_FACTOR) / 4
             */
             // just truncate fractions for now
             long new_production = (long) (map.grid[y_coord][x_coord].production * BLUR_FACTOR
                                            + map.grid[y_coord][left_coord].production * (1 - BLUR_FACTOR) / 4
                                            + map.grid[y_coord][right_coord].production * (1 - BLUR_FACTOR) / 4
                                            + map.grid[above_coord][x_coord].production * (1 - BLUR_FACTOR) / 4
                                            + map.grid[below_coord][x_coord].production * (1 - BLUR_FACTOR) / 4);
             return new_production;
        }

        std::string TileGenerator::name() const {
            return "Tile Generator";
        }


        hlt::Map TileGenerator::generate(std::list<hlt::Player> &players) {

            auto map = Map(width, height);
            for (long row = 0; row < tile_height; ++row) {
                for (long col = 0; col < tile_width; ++col) {
                    // randomly generate a production value
                    // TODO explain usage of this distribution function
                    // Also, could inherit/use class that generates a single tile for differently generated maps
                    const auto min_cell = hlt::MapConstants::get().MIN_CELL_PRODUCTION;
                    const auto max_cell = hlt::MapConstants::get().MIN_CELL_PRODUCTION;
                    unsigned long production = (unsigned long) rng() / rng.max() * (max_cell - min_cell) + min_cell;
                    // TODO update with Charles' new classes post pull request
                    map.grid[row][col].production = production;
                    map.grid[row][col].type = Cell::CellType::Normal;
                }
            }

            // Assign factory location randomly within square to start with
            // TODO: determine alternate method/modular method of factory location
            // TODO: check for best random distribution / long vs. int
            unsigned long factory_pos_x = (unsigned long) rng() / rng.max() * tile_width;
            unsigned long factory_pos_y = (unsigned long) rng() / rng.max() * tile_height;

            // Tile the whole map
            // TODO: assess algorithm for cache performance
            for (long player_row = 0; player_row < num_tile_rows; ++player_row) {
                for (long player_col = 0; player_col < num_tile_cols; ++player_col) {
                    for (long tile_row = 0; tile_row < tile_height; ++tile_row) {
                        for (long tile_col = 0; tile_col < tile_width; ++tile_col) {
                            map.grid[player_row * tile_height + tile_row][player_col * tile_width + tile_col] =
                                    map.grid[tile_row][tile_col];
                        }
                    }
                }
            }

            long player_idx = 0;
            for (auto &player : players) {
                const long player_factory_x = (player_idx % num_tile_cols) + factory_pos_x;
                const long player_factory_y = (player_idx / num_tile_cols) + factory_pos_y;
                map.grid[player_factory_y][player_factory_x] =
                        player.factory_cell;
                player_idx++;
            }

            // Blur the whole map
            for (long row = 0; row < height; ++row) {
                for (long col = 0; col < width; ++col) {
                    map.grid[row][col].production = blur_function(row, col, map);
                }
            }

            return map;
        }

        TileGenerator::TileGenerator(const MapParameters &parameters) :
                Generator(parameters),
                width(parameters.width),
                height(parameters.height),
                num_players(parameters.num_players) {

            // Ensure that the map can be subdivided into partitions for a given number of players
            // ie: a 64x64 map cannot be (most basic definition of) symmetrical for 6 players
            assert((width * height) % num_players == 0);

            // We want our map to be as square-like as possible, so to find the size of a tile, first determine
            // how many tiles we will have
            num_tile_rows = (unsigned long) sqrt(num_players);
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
