#include "hlt.hpp"

#include <random>

int main() {
    // Get the initial game state
    id_type myID;
    hlt::Players players;
    hlt::Map map;
    hlt::getInit(map, players, myID);

    // Respond with our name.
    hlt::sendInit("MyC++Bot-" + std::to_string(myID));

    // Initialize the pseudorandom generator.
    std::mt19937 prg(time(NULL));

    while (true) {
        // Get the newest frame.
        unsigned int turn_number = hlt::getFrame(map, players);
        // For each of our ships
        for (const auto &id_ship : players[myID].ships) {
            const auto id = id_ship.first;
            const auto &ship = id_ship.second;
            // If we're on our shipyard and have enough halite, dump it.
            if (ship.location == players[myID].shipyard && ship.halite > hlt::MAX_HALITE / 4) {
                hlt::dump(id, ship.halite);
            }
            // Otherwise, check if there's a reasonable amount of halite on the square and we have capacity.
            // If so, extract halite. If not, move randomly.
            else if (map[ship.location] > hlt::MAX_HALITE / 10 &&
                     hlt::MAX_HALITE - ship.halite >= map[ship.location] / 10) {
                continue; // Do nothing, which is to say, extract halite.
            } else {
                hlt::move(id, hlt::CARDINALS[prg() % 4]); // Move in a random direction
            }
        }

        // If we're in the first 200 turns and have enough halite, spawn a ship.
        if (turn_number <= 200 && players[myID].halite >= hlt::SHIP_COST) {
            hlt::spawn(0); // We don't want to put any halite in the newly spawned ship.
        }

        // Send our moves back to the game environment
        hlt::sendFrame();
    }

    return 0;
}
