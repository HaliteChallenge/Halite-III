#ifndef GENERATOR_H
#define GENERATOR_H

#ifdef _WIN32
#define _USE_MATH_DEFINES
#include <cmath>
#endif

#include <list>
#include <random>

#include "Player.hpp"

#include "util/json.hpp"

namespace hlt {
namespace mapgen {

/** Types of available maps to be generated. */
enum class MapType {
    Basic
};

/** Parameters for map generation. */
struct MapParameters {
    MapType type; /**< Type of the map. */
    unsigned int seed; /**< Random seed. */
    unsigned int width; /**< Width of the map. */
    unsigned int height; /**< Height of the map. */
    unsigned int num_players; /**< Number of players for which to generate the map. */
};

/** Base class for Halite map generators. */
class Generator {
protected:
    std::mt19937 rng;

    /** Share the friend Map constructor with all Generator subclasses. */
    hlt::Map Map(int width, int height) const;

    explicit Generator(const MapParameters &parameters);

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
