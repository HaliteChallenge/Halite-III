#include "Grid.hpp"

#include "nlohmann/json.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, grid.x}

namespace hlt {

/**
 * Convert a Grid to JSON format.
 * @tparam Entry The type of the grid entries.
 * @param[out] json The output JSON.
 * @param grid The Grid to convert.
 */
template<class Entry>
void to_json(nlohmann::json &json, const Grid<Entry> &grid) {
    json = {FIELD_TO_JSON(width),
            FIELD_TO_JSON(height),
            FIELD_TO_JSON(grid)};
}

}
