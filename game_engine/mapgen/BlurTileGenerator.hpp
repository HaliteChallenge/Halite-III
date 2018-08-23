#ifndef BLURTILEGENERATOR_H
#define BLURTILEGENERATOR_H

#include "TileGenerator.hpp"
#include "Map.hpp"

namespace hlt {
namespace mapgen {
/**
 * Map generator using a "tiling" method. This generates a single tile of a map, and then duplicates
 * that tile for each player, arranging the tiles into an appropriate number of rows and columns.
 * The size of the tile is determined from the desired total height and width of the map,
 * and the number of players.
 */
class BlurTileGenerator : public TileGenerator {
private:
    /**
     * Generate a production value correlated with neighboring cells
     * @param y_coord: y_coordinate of cell of interest
     * @param x_coord: x_coordinate of cell of interest
     * @param map: map of the cells. Assumes map is at least as large as tile_width x tile_height
     * @return new production value for the cell of interest
     */
    energy_type blur_function(dimension_type y_coord, dimension_type x_coord, const hlt::Map &map) const;

public:
    std::string name() const override { return "Blur Tile"; };

    /**
     * Generate a map.
     * @param[out] map The map to generate.
     */
    void generate(Map &map) override;

    /**
     * Construct BlurTileGenerator from parameters.
     * @param parameters: The map generation parameters.
     */
    explicit BlurTileGenerator(const MapParameters &parameters) :
            TileGenerator(parameters) {};
};
}
}

#endif // BLURTILEGENERATOR_H
