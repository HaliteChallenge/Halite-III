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

        for(auto &[loc, _] : players[myID].entities) {
            switch (prg() % 4) {
                case 0:
                    moves[loc] = hlt::Direction::NORTH;
                    break;
                case 1:
                    moves[loc] = hlt::Direction::EAST;
                    break;
                case 2:
                    moves[loc] = hlt::Direction::SOUTH;
                    break;
                case 3:
                    moves[loc] = hlt::Direction::WEST;
                    break;
            }
            
        }

        hlt::sendFrame(moves);
    }

    return 0;

}
