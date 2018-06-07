#ifndef CELL_HPP
#define CELL_HPP

#include <iostream>

#include "Entity.hpp"

#include "util/json.hpp"

namespace hlt {

/** A cell on the grid. */
struct Cell {
    /** The types of cells. */
    enum class CellType {
        Normal = 0,
        Factory = 1,
    };

    CellType type;
    long production;
    bool impassable;

    friend void to_json(nlohmann::json &json, const Cell &cell);

    friend void from_json(const nlohmann::json &json, Cell &cell);

    friend std::ostream &operator<<(std::ostream &os, const Cell &cell);

    Cell(const CellType &type, long production, bool impassable = false);

    Cell();
};

}


#endif // CELL_HPP
