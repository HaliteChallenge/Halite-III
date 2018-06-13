#include "BotCommunicationError.hpp"
#include "Location.hpp"
#include "Map.hpp"

#include "nlohmann/json.hpp"

/** The JSON key for x-coordinate. */
constexpr auto JSON_POS_X = "pos_x";
/** The JSON key for y-coordinate. */
constexpr auto JSON_POS_Y = "pos_y";

namespace hlt {

/**
 * Read a Direction from bot serial format.
 * @param istream The input stream.
 * @param[out] direction The direction to read.
 * @return The input stream.
 */
std::istream &operator>>(std::istream &istream, Direction &direction) {
    // Read one character corresponding to the type, and dispatch based on its value.
    char direction_type;
    istream >> direction_type;
    switch (direction_type) {
    case static_cast<char>(Direction::North):
        direction = Direction::North;
        break;
    case static_cast<char>(Direction::South):
        direction = Direction::South;
        break;
    case static_cast<char>(Direction::East):
        direction = Direction::East;
        break;
    case static_cast<char>(Direction::West):
        direction = Direction::West;
        break;
    default:
        throw BotCommunicationError(std::string() + direction_type);
    }
    return istream;
}

/**
 * Manhattan distance to another Location.
 * @param other The other Location.
 */
long Location::distance(const Location &other) const {
    // TODO: wrap around edges
    return abs(other.pos_x - pos_x) + abs(other.pos_y - pos_y);
}

/**
 * Move this Location in one direction.
 * @param direction The Direction in which to move.
 * @param map The Map on which to move.
 */
void Location::move_toward(const Direction &direction, const Map &map) {
    switch (direction) {
    case Direction::North:
        pos_y = (pos_y + map.height - 1) % map.height;
        break;
    case Direction::South:
        pos_y = (pos_y + 1) % map.height;
        break;
    case Direction::East:
        pos_x = (pos_x + 1) % map.width;
        break;
    case Direction::West:
        pos_x = (pos_x + map.width - 1) % map.width;
        break;
    }
}

/**
 * Convert a Location to JSON format.
 * @param[out] json The output JSON.
 * @param location The Location to convert.
 */
void to_json(nlohmann::json &json, const Location &location) {
    json = {{JSON_POS_X, location.pos_x},
            {JSON_POS_Y, location.pos_y}};
}

/**
 * Convert an encoded Location from JSON format.
 * @param json The JSON.
 * @param[out] location The converted Location.
 */
void from_json(const nlohmann::json &json, Location &location) {
    location = {json.at(JSON_POS_X).get<decltype(location.pos_x)>(),
                json.at(JSON_POS_Y).get<decltype(location.pos_y)>()};
}

}
