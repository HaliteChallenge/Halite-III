#include "player.hpp"
#include "input.hpp"

void hlt::Player::_update(int num_ships, int num_dropoffs, Halite halite_amount) {
    this->halite_amount = halite_amount;
}

hlt::Player* hlt::Player::_generate() {
/*
    long num_ships;
    long num_dropoffs;
    in >> player.id >> num_ships >> num_dropoffs >> player.halite_amount;

    player.ships.clear();
    for(int i = 0; i < num_ships; i++) {
        EntityId id;
        Position position;
        Halite halite_amount;
        in >> id >> position >> halite_amount;
        player.ships[id] = { id, position, halite_amount };
    }

    player.dropoffs.clear();
    for(int i = 0; i < num_dropoffs; i++) {
        EntityId id;
        Position position;
        in >> id >> position;
        player.dropoffs[id] = { id, position };
    }
 */
}
