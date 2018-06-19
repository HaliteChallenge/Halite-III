#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "nlohmann/json_fwd.hpp"
#include "Constants.hpp"

namespace hlt {

/** The four cardinal directions. */
enum class Direction : char {
    North = 'n',
    South = 's',
    East = 'e',
    West = 'w',
};

/**
 * Convert a Direction to JSON format.
 * @param[out] json The output JSON.
 * @param direction The Direction to convert.
 */
void to_json(nlohmann::json &json, const Direction &direction);

/**
 * Convert an encoded Direction from JSON format.
 * @param json The JSON.
 * @param[out] direction The converted Direction.
 */
void from_json(const nlohmann::json &json, Direction &direction);

/**
 * Read a Direction from bot serial format.
 * @param istream The input stream.
 * @param[out] direction The direction to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, Direction &direction);

/** Type of a location. */
using Location = std::pair<dimension_type, dimension_type>;

}

#endif // LOCATION_HPP
