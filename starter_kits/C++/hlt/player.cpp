#include "player.hpp"
#include "input.hpp"

void hlt::Player::_update(int num_ships, int num_dropoffs, Halite halite) {
    this->halite = halite;

    ships.clear();
    for (int i = 0; i < num_ships; ++i) {
        std::shared_ptr<hlt::Ship> ship = hlt::Ship::_generate(id);
        ships[ship->id] = ship;
    }

    dropoffs.clear();
    for (int i = 0; i < num_dropoffs; ++i) {
        std::shared_ptr<hlt::Dropoff> dropoff = hlt::Dropoff::_generate(id);
        dropoffs[dropoff->id] = dropoff;
    }
}

std::shared_ptr<hlt::Player> hlt::Player::_generate() {
    PlayerId player_id;
    int shipyard_x;
    int shipyard_y;
    hlt::get_sstream() >> player_id >> shipyard_x >> shipyard_y;

    return std::make_shared<hlt::Player>(player_id, shipyard_x, shipyard_y);
}
