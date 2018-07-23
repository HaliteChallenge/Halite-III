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

/**
 * Read a MapType from command line input.
 * @param istream The input stream.
 * @param[out] type The MapType to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, MapType &type);
std::ostream &operator<<(std::ostream &ostream, MapType type);

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
     */
    virtual void generate(Map &map) = 0;

    /**
     * Generate a map based on parameters.
     * @param[out] map The map to generate.
     * @param parameters The parameters to use.
     */
    static void generate(Map &map, const MapParameters &parameters);
};

}
}

#endif //GENERATOR_H
