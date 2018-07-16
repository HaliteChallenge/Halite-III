#ifndef CELL_HPP
#define CELL_HPP

#include <iostream>

#include "Entity.hpp"

#include "nlohmann/json_fwd.hpp"

namespace hlt {

/** Cell on the Halite map. */
struct Cell {
    /** Energy of this Cell. */
    energy_type energy{};

    /** Entity on this Cell. */
    Entity::id_type entity = Entity::None;

    /**
     * Convert a Cell to JSON format.
     * @param[out] json The output JSON.
     * @param cell The cell to convert.
     */
    friend void to_json(nlohmann::json &json, const Cell &cell);

    /**
     * Write a Cell to bot serial format.
     * @param ostream The output stream.
     * @param cell The cell to write.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const Cell &cell);
};

}

#endif // CELL_HPP
