#include "hlt.hpp"

#include <random>
#include <chrono>

int main() {
    std::mt19937 prg = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());

    std::cout.sync_with_stdio(0);

    id_type myID;
    hlt::Players players;
    hlt::Map map;
    hlt::getInit(map, players, myID);
    hlt::sendInit("MyC++Bot-"+std::to_string(myID));

    hlt::Moves moves;
    while(true) {
        moves.clear();

        hlt::getFrame(players);

        for(auto &[loc, entity] : players[myID].entities) {
            moves[loc] = hlt::Direction::NORTH;
        }

        hlt::sendFrame(moves);
    }

    return 0;

}
