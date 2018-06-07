#include "Map.hpp"

namespace hlt {

Map::Map(int width, int height) : width(width), height(height) {
    grid.resize((size_t) height, std::vector<Cell>((size_t) width));
}

void to_json(nlohmann::json &json, const Map &map) {
    json = {{"width",  map.width},
            {"height", map.height},
            {"grid",   map.grid}};
}

void from_json(const nlohmann::json &json, Map &map) {
    map = {json.at("width").get<int>(),
           json.at("height").get<int>(),
           json.at("grid").get<Map::Grid>()};
}

std::ostream &operator<<(std::ostream &os, const Map &map) {
    os << map.width << " " << map.height << std::endl;
    for (const auto &row : map.grid) {
        for (const auto &cell : row) {
            os << cell;
        }
    }
    return os;
}

Map::Map(int width, int height, Map::Grid grid) : width(width), height(height), grid(std::move(grid)) {}

}