#include "BotCommunicationError.hpp"
#include "JsonError.hpp"
#include "Location.hpp"
#include "Map.hpp"

#include "nlohmann/json.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, location.x}

/** Get a field from JSON. */
#define FIELD_FROM_JSON(x) json.at(#x)

namespace hlt {

/**
 * Convert a Direction to JSON format.
 * @param[out] json The output JSON.
 * @param direction The Direction to convert.
 */
void to_json(nlohmann::json &json, const Direction &direction) {
    json = std::string() + static_cast<char>(direction);
}

/**
 * Convert a char to a Direction.
 * @param direction_char The character.
 * @param[out] direction The converted direction.
 * @return true if the conversion was successful.
 */
bool from_char(const char direction_char, Direction &direction) {
    switch (direction_char) {
    case static_cast<char>(Direction::North):
        direction = Direction::North;
        return true;
    case static_cast<char>(Direction::South):
        direction = Direction::South;
        return true;
    case static_cast<char>(Direction::East):
        direction = Direction::East;
        return true;
    case static_cast<char>(Direction::West):
        direction = Direction::West;
        return true;
    default:
        return false;
    }
}

/**
 * Convert an encoded Direction from JSON format.
 * @param json The JSON.
 * @param[out] direction The converted Direction.
 */
void from_json(const nlohmann::json &json, Direction &direction) {
    std::string string = json;
    if (string.empty()) {
        throw BotCommunicationError(string);
    }
    char direction_type = string.front();
    if (!from_char(direction_type, direction)) {
        throw JsonError(std::string() + direction_type);
    }
}

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
    if (!from_char(direction_type, direction)) {
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
    json = {FIELD_TO_JSON(pos_x), FIELD_TO_JSON(pos_y)};
}

/**
 * Convert an encoded Location from JSON format.
 * @param json The JSON.
 * @param[out] location The converted Location.
 */
void from_json(const nlohmann::json &json, Location &location) {
    location = {FIELD_FROM_JSON(pos_x), FIELD_FROM_JSON(pos_y)};
}

}
