#ifndef BASICGENERATOR_H
#define BASICGENERATOR_H

#include "Generator.hpp"

namespace hlt {
namespace mapgen {

class BasicGenerator : Generator {
private:
    unsigned int width;
    unsigned int height;
    unsigned int num_players;
public:
    std::string name() const override;

    hlt::Map generate(std::list<Player> &players) override;

    explicit BasicGenerator(const MapParameters &parameters);
};

}
}

#endif // BASICGENERATOR_H
