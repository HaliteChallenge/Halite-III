#include "Generator.hpp"
#include "BasicGenerator.hpp"
#include "Map.hpp"

namespace hlt {
namespace mapgen {

Generator::Generator(const MapParameters &parameters) {
    rng = std::mt19937(parameters.seed);
}

hlt::Map Generator::Map(int width, int height) {
    return hlt::Map::Map(width, height);
}

}
}
