#ifndef SYMMETRICALTILE_H
#define SYMMETRICALTILE_H

#include "Generator.hpp"
#include "Location.hpp"

namespace hlt {
namespace mapgen {
/**
 * Map generator that creates a symmetrical map via repeated reflection of a single starting tile.
 */
class SymmetricalTile : public Generator {
    dimension_type num_tile_rows; /**< number of rows of tiles in map */
    dimension_type num_tile_cols; /**< number of cols of tiles in map */
    dimension_type width;         /**< width (in cells) of the final map */
    dimension_type height;        /**< height (in cells) of the final map */

    /**
     * Reflects the upper left hand corner of map across the y axis
     *
     * @param map Map to initialize. Assumes upper left corner already initialized. Will also double number of factories owned by map
     * @param tile_width Width of segment of map already filled in. Assumes map filled beginning at 0, 0
     * @param tile_height Height of segment of map already filled in. Assumes map filled beginning at 0, 0
     */
    void flip_vertical(Map &map, dimension_type tile_width, dimension_type tile_height);

    /**
     * Reflects the upper left hand corner of map across the x axis
     *
     * @param map Map to initialize. Assumes upper left corner already initialized. Will also double number of factories owned by map
     * @param tile_width Width of segment of map already filled in. Assumes map filled beginning at 0, 0
     * @param tile_height Height of segment of map already filled in. Assumes map filled beginning at 0, 0
     */
    void flip_horizontal(Map &map, dimension_type tile_width, dimension_type tile_height);
protected:
    unsigned long num_players;    /**< number of players who will be on the map */
    dimension_type tile_width;    /**< width (in cells) of a single tile */
    dimension_type tile_height;   /**< width (in cells) of a single tile */


    /** Tile a map from a single tile via repeated reflection
     *
     * @param[out] map A map tiled by the input tile. All cells in the map will be initialized.
     * @param factory_y, factory_x: IGNORED - generates own factory location
     * @param tile: A filled map of a single tile with dimensions tile_height, tile_width. All cells should already be
     * initialized, but no factories should be placed
     */
    void tile_map(Map &map, dimension_type factory_y, dimension_type factory_x,
                  const Map &tile);

public:
    /** name is function to allow for possibly dynamically named subclasses */
    std::string name() const override { return "flip tile"; };


    /** Symmetrical Tile class constructor
     *
     * @param parameters: Map parameters as defined in Generator.hpp
     * Note: dynamically calculates tile height and width based on overall height and width and
     * number of players. Will cause an assertion error if map for number of players cannot be created via repeated reflection
     */
    explicit SymmetricalTile(const MapParameters &parameters);

};
}
}


#endif // SYMMETRICALTILE_H
