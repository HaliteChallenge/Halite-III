#include "Location.hpp"
#include "Map.hpp"

#include "util/json.hpp"

namespace hlt {

std::istream &operator>>(std::istream &is, Direction &direction) {
    char d;
    is >> d;
    switch (d) {
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
        break;
    }
    return is;
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

bool operator==(const Location &l1, const Location &l2) {
    return l1.pos_x == l2.pos_x && l1.pos_y == l2.pos_y;
}

Location::Location(long pos_x, long pos_y) : pos_x(pos_x), pos_y(pos_y) {}

Location::Location() = default;

}