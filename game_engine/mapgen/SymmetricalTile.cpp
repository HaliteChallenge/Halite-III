#include "SymmetricalTile.hpp"

#include <unordered_set>

#include "Logging.hpp"

namespace hlt {
namespace mapgen {

/** Symmetrical Tile class constructor
 *
 * @param parameters: Map parameters as defined in Generator.hpp
 * Note: dynamically calculates tile height and width based on overall height and width and
 * number of players. Will cause an assertion error if map for number of players cannot be created via repeated reflection
 */
SymmetricalTile::SymmetricalTile(const MapParameters &parameters) :
        Generator(parameters),
        width(parameters.width),
        height(parameters.height),
        num_players(parameters.num_players) {

    // Ensure that the map can be created in symmetrical fashion
    const std::unordered_set<unsigned long> ALLOWED_NUMBERS({1, 2, 4, 8, 16});
    if (ALLOWED_NUMBERS.find(num_players) == ALLOWED_NUMBERS.end()) {
        std::ostringstream stream;
        stream << "We cannot create a game map for the specified number of players (" << num_players << "). ";
        stream << "Please try playing with 1, 2, 4, 8, or 16 players";
        Logging::log(stream.str(), Logging::Level::Error);
    }
    assert(ALLOWED_NUMBERS.find(num_players) != ALLOWED_NUMBERS.end());

    unsigned long num_tiles = 1;
    num_tile_rows = 1;
    num_tile_cols = 1;
    while (num_tiles < num_players) {
        // Flips will happen across y axis, then x axis, repeatedly
        num_tile_cols *= 2;
        num_tiles *= 2;
        if (num_tiles == num_players) break;
        num_tile_rows *=2;
        num_tiles *= 2;
    }

    // next, use number of rows and cols of player start tiles to determine height and width of a single tile
    tile_width = width / num_tile_cols;
    tile_height = height / num_tile_rows;

    // Ensure these tiles then make up the whole map
    if (!(tile_width * num_tile_cols == width && tile_height * num_tile_rows == height)) {
        std::ostringstream stream;
        stream << "We cannot create a symmetrical map of dimensions " << width << " x " << height << " for " << num_players;
        stream << " players. Try using dimensions that are a power of 2 instead.";
        Logging::log(stream.str(), Logging::Level::Error);
    }
    assert(tile_width * num_tile_cols == width && tile_height * num_tile_rows == height);
}

/**
 * Reflects the upper left hand corner of map across the y axis
 *
 * @param map Map to initialize. Assumes upper left corner already initialized. Will also double number of factories owned by map
 * @param tile_width Width of segment of map already filled in. Assumes map filled beginning at 0, 0
 * @param tile_height Height of segment of map already filled in. Assumes map filled beginning at 0, 0
 */
void SymmetricalTile::flip_vertical(Map &map, const dimension_type tile_width, const dimension_type tile_height) {
    // Reflect filled in part of map over y axis
    for (dimension_type tile_row = 0; tile_row < tile_height; ++tile_row) {
        for (dimension_type tile_col = 0; tile_col < tile_width; ++tile_col) {
            map.at(tile_width * 2 - tile_col - 1, tile_row).energy =  map.at(tile_col, tile_row).energy;
        }
    }

    // add reflections of current factories
    std::vector<Location> factory_locations = map.factories;
    for (const auto factory : factory_locations) {
        const Location flipped_factory = Location(tile_width * 2 - factory.x - 1, factory.y);
        map.at(flipped_factory).energy = 0;
        map.factories.push_back(flipped_factory);
    }
}

/**
 * Reflects the upper left hand corner of map across the x axis
 *
 * @param map Map to initialize. Assumes upper left corner already initialized. Will also double number of factories owned by map
 * @param tile_width Width of segment of map already filled in. Assumes map filled beginning at 0, 0
 * @param tile_height Height of segment of map already filled in. Assumes map filled beginning at 0, 0
 */
void SymmetricalTile::flip_horizontal(Map &map, const dimension_type tile_width, const dimension_type tile_height) {
    // reflect filled in part of map over x axis
    for (dimension_type tile_row = 0; tile_row < tile_height; ++tile_row) {
        for (dimension_type tile_col = 0; tile_col < tile_width; ++tile_col) {
            map.at(tile_col, tile_height * 2 - tile_row - 1).energy = map.at(tile_col, tile_row).energy;
        }
    }

    // add reflections of current factories
    std::vector<Location> factory_locations = map.factories;
    for (const auto factory : factory_locations) {
        const Location flipped_factory = Location(factory.x, tile_height * 2 - factory.y - 1);
        map.at(flipped_factory).energy = 0;
        map.factories.push_back(flipped_factory);
    }
}

/** Tile a map from a single tile via repeated reflection
 *
 * @param[out] map A map tiled by the input tile. All cells in the map will be initialized.
 * @param factory_y, factory_x: IGNORED - generates own factory location
 * @param tile: A filled map of a single tile with dimensions tile_height, tile_width. All cells should already be
 * initialized, but no factories should be placed
 */
void SymmetricalTile::tile_map(hlt::Map &map, hlt::dimension_type factory_y, hlt::dimension_type factory_x,
                               const hlt::Map &tile) {
    // Put factory in the center (ish) of tile (closer to center of map)
    dimension_type factory_pos_x = tile.width / 2;
    dimension_type factory_pos_y = tile.height / 2;
    if (tile.width >= 16 && tile.width <= 40 && tile.height >= 16 && tile.height <= 40) {
        factory_pos_x = static_cast<dimension_type>(8 + ((tile.width - 16) / 24.0) * 20);
        if (num_players > 2) {
            factory_pos_y = static_cast<dimension_type>(8 + ((tile.height - 16) / 24.0) * 20);
        }
    }
    (void) factory_y, (void) factory_x;

    // copy tile and factory onto map
    for (dimension_type tile_row = 0; tile_row < tile_height; ++tile_row) {
        for (dimension_type tile_col = 0; tile_col < tile_width; ++tile_col) {
            map.at(tile_col, tile_row).energy = tile.at(tile_col, tile_row).energy;
        }
    }

    const Location factory = Location(factory_pos_x, factory_pos_y);
    map.at(factory).energy = 0;
    map.factories.push_back(factory);

    unsigned long num_tiles = 1;
    dimension_type curr_width = tile_width;
    dimension_type curr_height = tile_height;
    while (num_tiles < num_players) {
        // Flips will happen across y axis, then x axis, repeatedly, until whole map filled in
        flip_vertical(map, curr_width, curr_height);
        curr_width *= 2;
        num_tiles *= 2;
        if (num_tiles == num_players) break;
        flip_horizontal(map, curr_width, curr_height);
        curr_height *=2;
        num_tiles *= 2;
    }
}

}
}
