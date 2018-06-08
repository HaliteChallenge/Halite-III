#include "BasicGenerator.hpp"
#include "Generator.hpp"
#include "Map.hpp"

namespace hlt {
namespace mapgen {

Generator::Generator(const MapParameters &parameters) {
    rng = std::mt19937(parameters.seed);
}

hlt::Map Generator::Map(long width, long height) const {
    return hlt::Map::Map(width, height);
}

}
}
