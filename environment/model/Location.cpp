#include "Location.hpp"
#include "Map.hpp"

namespace hlt {

std::istream &operator>>(std::istream &istream, Direction &direction) {
    char direction_type;
    istream >> direction_type;
    switch (direction_type) {
    case 'N':
        direction = Direction::North;
        break;
    case 'S':
        direction = Direction::South;
        break;
    case 'E':
        direction = Direction::East;
        break;
    case 'W':
        direction = Direction::West;
        break;
    default:
        // TODO: error case
        break;
    }
    return istream;
}

long Location::distance(const Location &other) const {
    return abs(other.pos_x - pos_x) + abs(other.pos_y - pos_y);
}

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

void to_json(nlohmann::json &json, const Location &location) {
    json = {{"pos_x", location.pos_x},
            {"pos_y", location.pos_y}};
}

void from_json(const nlohmann::json &json, Location &location) {
    location = {json.at("pos_x").get<long>(),
                json.at("pos_y").get<long>()};
}

}
