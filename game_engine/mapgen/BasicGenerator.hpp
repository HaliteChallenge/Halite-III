#ifndef BASICGENERATOR_H
#define BASICGENERATOR_H

#include "Generator.hpp"

namespace hlt {
namespace mapgen {

class BasicGenerator : public Generator {
private:
    long num_players;  /**< The number of players. */
public:
    std::string name() const override { return "basic"; }

    void generate(Map &map) override;

    /**
     * Construct BasicGenerator from parameters.
     * @param parameters The map generation parameters.
     */
    explicit BasicGenerator(const MapParameters &parameters) :
            Generator(parameters), num_players(parameters.num_players) {}
};

}
}

#endif // BASICGENERATOR_H
