#ifndef GENERATOR_H
#define GENERATOR_H

#include <list>
#include <random>

#include "Player.hpp"
#include "Map.hpp"

namespace hlt {
namespace mapgen {

/** Types of available maps to be generated. */
enum class MapType {
    Basic
};

/** Parameters for map generation. */
struct MapParameters {
    MapType type;               /**< Type of the map. */
    unsigned int seed;          /**< Random seed. */
    Map::dimension_type width;  /**< Width of the map. */
    Map::dimension_type height; /**< Height of the map. */
    unsigned long num_players;  /**< Number of players for which to generate the map. */
};

/** Base class for Halite map generators. */
class Generator {
protected:
    /** The random number generator. */
    std::mt19937 rng;

    /**
     * Share the friend Map constructor with all Generator subclasses.
     * @param width The width of the Map to construct.
     * @param height The height of the Map to construct.
     * @return The constructed Map.
     */
    static hlt::Map Map(Map::dimension_type width, Map::dimension_type height) {
        return hlt::Map::Map(width, height);
    }

    /**
     * Construct Generator from parameters.
     * @param parameters The map generation parameters.
     */
    explicit Generator(const MapParameters &parameters) : rng(std::mt19937(parameters.seed)) {}

public:
    /**  Get the name of this map generator. */
    virtual std::string name() const = 0;

    /**
     * Generate a map based on a list of players.
     * @param players The players on the map.
     */
    virtual hlt::Map generate(std::list<hlt::Player> &players) const = 0;
};

}
}

#endif //GENERATOR_H
