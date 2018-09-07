#include "ship.hpp"
#include "input.hpp"

std::shared_ptr<hlt::Ship> hlt::Ship::_generate(hlt::PlayerId player_id) {
    hlt::EntityId ship_id;
    int x;
    int y;
    hlt::Halite halite;
    hlt::get_sstream() >> ship_id >> x >> y >> halite;

    return std::make_shared<hlt::Ship>(player_id, ship_id, x, y, halite);
}
