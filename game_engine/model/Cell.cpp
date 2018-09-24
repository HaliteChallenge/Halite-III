#include "Cell.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, cell.x}

namespace hlt {

/**
 * Write a Cell to bot serial format.
 * @param ostream The output stream.
 * @param cell The cell to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Cell &cell) {
    return ostream << std::to_string(cell.energy);
}

/**
 * Convert a Cell to JSON format.
 * @param[out] json The JSON output.
 */
void to_json(nlohmann::json &json, const Cell &cell) {
    json = {FIELD_TO_JSON(energy)};
}

}
