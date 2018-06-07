#include "Cell.hpp"

namespace hlt {

void to_json(nlohmann::json &json, const Cell &cell) {
    json = {{"type",       static_cast<int>(cell.type)},
            {"production", cell.production},
            {"impassable", cell.impassable}};
}

void from_json(const nlohmann::json &json, Cell &cell) {
    cell = {static_cast<Cell::CellType>(json.at("type").get<int>()),
            json.at("production").get<long>(),
            json.at("impassable").get<bool>()};
}

std::ostream &operator<<(std::ostream &os, const Cell &cell) {
    return os << static_cast<int>(cell.type) << " " << cell.production << std::endl;
}

Cell::Cell(const Cell::CellType &type, long production, bool impassable) :
        type(type), production(production), impassable(impassable) {}

Cell::Cell() = default;

}