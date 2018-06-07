#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <unordered_map>
#include <vector>

#include "Cell.hpp"
#include "Entity.hpp"
#include "Generator.hpp"
#include "Player.hpp"

namespace hlt {

/** The map of cells. */
class Map {
    friend class hlt::mapgen::Generator;
    using Grid = std::vector<std::vector<Cell>>;

public:
    int width, height;

    EntityFactory entity_factory;
    Grid grid;

    friend void to_json(nlohmann::json &json, const Map &map);

    friend void from_json(const nlohmann::json &json, Map &map);

    friend std::ostream &operator<<(std::ostream &os, const Map &map);

private:
    Map(int width, int height);

    Map(int width, int height, Grid grid);
};

}

#endif /* MAP_H */
