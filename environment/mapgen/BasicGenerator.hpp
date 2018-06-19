#ifndef BASICGENERATOR_H
#define BASICGENERATOR_H

#include "Generator.hpp"

namespace hlt {
namespace mapgen {

class BasicGenerator : public Generator {
private:
    hlt::dimension_type width;       /**< The width of the map. */
    hlt::dimension_type height;      /**< The height of the map. */
public:
    std::string name() const override { return "basic"; }

    hlt::Map generate(std::unordered_map<id_type, Player> &players) override;

    /**
     * Construct BasicGenerator from parameters.
     * @param parameters The map generation parameters.
     */
    explicit BasicGenerator(const MapParameters &parameters) :
            Generator(parameters), width(parameters.width), height(parameters.height) {}
};

}
}

#endif // BASICGENERATOR_H
