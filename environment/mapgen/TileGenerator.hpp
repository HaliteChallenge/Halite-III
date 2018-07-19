#ifndef TILEGENERATOR_H
#define TILEGENERATOR_H

#include "Generator.hpp"

namespace hlt {
namespace mapgen {
/**
 * Map generator using a "tiling" method. This generates a single tile of a map, and then duplicates
 * that tile for each player, arranging the tiles into an appropriate number of rows and columns.
 * The size of the tile is determined from the desired total height and width of the map,
 * and the number of players.
 *
 * TileGenerator class is intended to be a super class implementing tiling functionality for subclasses that
 * implement different methods of generating single tile
 */
class TileGenerator : public Generator {
    dimension_type num_tile_rows; /**< number of rows of tiles in map */
    dimension_type num_tile_cols; /**< number of cols of tiles in map */
    dimension_type width;         /**< width (in cells) of the final map */
    dimension_type height;        /**< height (in cells) of the final map */
protected:
    unsigned long num_players;    /**< number of players who will be on the map */
    dimension_type tile_width;    /**< width (in cells) of a single tile */
    dimension_type tile_height;   /**< width (in cells) of a single tile */


    /** Tile a map from a single tile
     *
     * @param[out] map A map tiled by the input tile. All cells in the map will be initialized.
     * @param factory_y, factory_x: On a tile, the y and x coordinate a factory should be placed
     * @param tile: A filled map of a single tile. Dimensions tile_height, tile_width. All cells should already be
     * initialized, but no factories should be placed
     */
    void tile_map(Map &map, dimension_type factory_y, dimension_type factory_x,
                  const Map &tile);

public:
    /** name is function to allow for possibly dynamically named subclasses */
    std::string name() const override { return "tile"; };


    /** TileGenerator class constructor
     *
     * @param parameters: Map parameters as defined in Generator.hpp
     * Note: dynamically calculates tile height and width based on  overall height and width and
     * number of players. Will cause an assertion error if given height and width cannot be divided into tiles
     * evenly (ie equally sized tiles arranged in a nice grid)
     */
    explicit TileGenerator(const MapParameters &parameters);

};
}
}

#endif // TILEGENERATOR_H
