#ifndef OWNERSHIPGRID_HPP
#define OWNERSHIPGRID_HPP

#include <unordered_set>

#include "Constants.hpp"
#include "Entity.hpp"
#include "Grid.hpp"

namespace hlt {

/** Representation of the owner of a cell on the grid. */
struct CellOwner {
    /** Special owner specifications. */
    enum {
        UNOWNED = -1,                               /** Represents a cell not owned by any player. */
        TIED = -2                                   /** Represents a cell owned by possibly multiple players. */
    };
    Player::id_type owner = UNOWNED;                /** The owner of the cell. */
    std::unordered_set<std::shared_ptr<Entity>> entities;  /** The entities leading to ownership of the cell. */
    dimension_type distance{};                      /** The distance to the entities leading to ownership. */
};

/** Representation of the map grid containing ownership information. */
class OwnershipGrid : public Grid<CellOwner> {
public:
    /**
     * Create an OwnershipGrid from dimensions.
     * @param width The width.
     * @param height The height.
     */
    OwnershipGrid(dimension_type width, dimension_type height) : Grid(width, height) {}
};

}

#endif // OWNERSHIPGRID_HPP
