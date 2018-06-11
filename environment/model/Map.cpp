#include "Map.hpp"

namespace hlt {

Map::Map(dimension_type width, dimension_type height) : width(width), height(height) {
    grid.resize(static_cast<decltype(grid)::size_type>(height));
    for (auto &row : grid) {
        row.resize(static_cast<std::remove_reference_t<decltype(row)>::size_type>(width));
    }
}

void to_json(nlohmann::json &json, const Map &map) {
    json = {{"width",  map.width},
            {"height", map.height},
            {"grid",   map.grid}};
}

void from_json(const nlohmann::json &json, Map &map) {
    map = {json.at("width").get<decltype(map.width)>(),
           json.at("height").get<decltype(map.height)>(),
           json.at("grid").get<decltype(map.grid)>()};
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

}
