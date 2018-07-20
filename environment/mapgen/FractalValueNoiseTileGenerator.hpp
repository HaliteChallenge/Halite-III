#ifndef FRACTALVALUENOISETILEGENERATOR_H
#define FRACTALVALUENOISETILEGENERATOR_H

#include "TileGenerator.hpp"
#include "SymmetricalTile.hpp"
#include "Map.hpp"

namespace hlt {
namespace mapgen {
/**
 * Map generator using a "tiling" method. This generates a single tile of a map, and then duplicates
 * that tile for each player, arranging the tiles into an appropriate number of rows and columns.
 * The size of the tile is determined from the desired total height and width of the map,
 * and the number of players.
 */
class FractalValueNoiseTileGenerator : public SymmetricalTile {
private:
    std::vector<std::vector<double> >
    generateSmoothNoise(const std::vector<std::vector<double> > &source_noise, dimension_type wavelength) const;

public:
    std::string name() const override { return "Fractal Value Noise Tile"; };

    /**
     * Generate a map.
     * @param[out] map The map to generate.
     */
    void generate(Map &map) override;

    /**
     * Construct BlurTileGenerator from parameters.
     * @param parameters: The map generation parameters.
     */
    explicit FractalValueNoiseTileGenerator(const MapParameters &parameters) :
            SymmetricalTile(parameters) {};
};
}
}

#endif // FRACTALVALUENOISETILEGENERATOR_H
