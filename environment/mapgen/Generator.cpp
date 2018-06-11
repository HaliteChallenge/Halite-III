#include "BasicGenerator.hpp"
#include "Generator.hpp"
#include "Map.hpp"

namespace hlt {
namespace mapgen {

Generator::Generator(const MapParameters &parameters) : rng(std::mt19937(parameters.seed)) {}

hlt::Map Generator::Map(Map::dimension_type width, Map::dimension_type height) {
    return hlt::Map::Map(width, height);
}

}
}
