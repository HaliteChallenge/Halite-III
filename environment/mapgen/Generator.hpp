#ifndef GENERATOR_H
#define GENERATOR_H

#include <random>

#include "Map.hpp"

namespace hlt {
namespace mapgen {

/** Types of available maps to be generated. */
enum class MapType {
    Basic,
    BlurTile,
    Fractal
};

/** Parameters for map generation. */
struct MapParameters {
    MapType type;               /**< Type of the map. */
    unsigned int seed;          /**< Random seed. */
    dimension_type width;       /**< Width of the map. */
    dimension_type height;      /**< Height of the map. */
    unsigned long num_players;  /**< Number of players for which to generate the map. */
};

/** Base class for Halite map generators. */
class Generator {
protected:
    /** The random number generator. */
    std::mt19937 rng;

    /**
     * Construct Generator from parameters.
     * @param parameters The map generation parameters.
     */
    explicit Generator(const MapParameters &parameters) : rng(parameters.seed) {}

public:
    /**  Get the name of this map generator. */
    virtual std::string name() const = 0;

    /**
     * Generate a map.
     * @param[out] map The map to generate.
     * @param[out] factories The factories generated on the map. Length must be >= number of players.
     */
    virtual void generate(Map &map, std::vector<Location> &factories) = 0;
};

}
}

#endif //GENERATOR_H
