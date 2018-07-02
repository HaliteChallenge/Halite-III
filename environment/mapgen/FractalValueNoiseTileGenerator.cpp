#include <cassert>

#include "FractalValueNoiseTileGenerator.hpp"

namespace hlt {
namespace mapgen {

// Would use Grid but the constructor is protected and that's a pain :(
std::vector< std::vector<double> > FractalValueNoiseTileGenerator::generateSmoothNoise(const std::vector< std::vector<double> > & source_noise, dimension_type wavelength) const {
    std::vector< std::vector<double> > mini_source(source_noise.size()/wavelength, std::vector<double>(source_noise[0].size()/wavelength, 0));
    for (dimension_type y = 0; y < mini_source.size(); y++) {
        for (dimension_type x = 0; x < mini_source[0].size(); x++) {
            mini_source[y][x] = source_noise[wavelength*y][wavelength*x];
        }
    }
    std::vector< std::vector<double> > smoothed_source(source_noise.size(), std::vector<double>(source_noise[0].size(), 0));
    for (dimension_type y = 0; y < source_noise.size(); y++) {
        dimension_type y_i = y/wavelength, y_f = (y/wavelength + 1) % source_noise.size();
        double vertical_blend = double(y)/wavelength - y_i;
        for (dimension_type x = 0; x < source_noise[0].size(); x++) {
            dimension_type x_i = x/wavelength, x_f = (x/wavelength + 1) % source_noise[0].size();
            double horizontal_blend = double(x)/wavelength - x_i;

            double top_blend = (1-horizontal_blend)*mini_source[y_i][x_i] + horizontal_blend*mini_source[y_i][x_f];
            double bottom_blend = (1-horizontal_blend)*mini_source[y_f][x_i] + horizontal_blend*mini_source[y_f][x_f];
            smoothed_source[y][x] = (1-vertical_blend)*top_blend + vertical_blend*bottom_blend;
        }
    }
    return smoothed_source;

}

Map FractalValueNoiseTileGenerator::generate(std::vector<Player> &players) {
    assert(players.size() <= num_players);
    auto tile = make_map(tile_width, tile_height);

    std::vector< std::vector<double> > source_noise(tile_height, std::vector<double>(tile_width, 0));
    std::vector< std::vector<double> > region = source_noise;

    const double PERSISTENCE = 0.7; // Determines relative weight of local vs global features.
    const double FACTOR_EXP = 2; // Determines spikiness of map. Higher values weight towards 0.
    std::uniform_real_distribution<double> urd(0.0, 1.0);
    for (dimension_type y = 0; y < tile_height; y++) {
        for (dimension_type x = 0; x < tile_width; x++) {
            source_noise[y][x] = pow(urd(rng), FACTOR_EXP);
        }
    }

    const int MAX_OCTAVE = floor(log2(std::min(tile_width, tile_height)))+1;
    double amplitude = 1;
    for (int octave = 2; octave <= MAX_OCTAVE; octave++) {
        auto smoothed_source = generateSmoothNoise(source_noise, round(pow(2, MAX_OCTAVE-octave)));
        for (dimension_type y = 0; y < tile_height; y++) {
            for (dimension_type x = 0; x < tile_width; x++) {
                region[y][x] += amplitude * smoothed_source[y][x];
            }
        }
        amplitude *= PERSISTENCE;
    }
    for (dimension_type y = 0; y < tile_height; y++) {
        for (dimension_type x = 0; x < tile_width; x++) {
            region[y][x] += amplitude * source_noise[y][x];
        }
    }

    // Make productions spikier using exponential. Also find max value.
    double max_value = 0;
    for (dimension_type y = 0; y < tile_height; y++) {
        for (dimension_type x = 0; x < tile_width; x++) {
            region[y][x] = pow(region[y][x], 2);
            if(region[y][x] > max_value) max_value = region[y][x];
        }
    }

    // Normalize to highest value 
    const energy_type MAX_CELL_PRODUCTION = rng() % (1+Constants::get().MAX_CELL_PRODUCTION-Constants::get().MIN_CELL_PRODUCTION) + Constants::get().MIN_CELL_PRODUCTION;
    for (dimension_type y = 0; y < tile_height; y++) {
        for (dimension_type x = 0; x < tile_width; x++) {
            region[y][x] *= MAX_CELL_PRODUCTION / max_value;
            tile.at(x, y) = make_cell<NormalCell>(round(region[y][x]));
        }
    }

    const auto factory_pos_x = static_cast<dimension_type>((rng() % tile_width));
    const auto factory_pos_y = static_cast<dimension_type>((rng() % tile_height));

    // Use superclass function to copy the tile over the entire map, including placing all factories
    Map tiled_map =  tile_map(factory_pos_y, factory_pos_x, tile, players);
    tiled_map.map_generator = this->name();
    return tiled_map;
}

}
}
