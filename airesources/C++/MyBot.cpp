#include "hlt.hpp"

#include <random>

/*
My pieces (& factory) push away my pieces
Opposing pieces (& factory) are a draw for my pieces
*/

int main() {
    std::cout.sync_with_stdio(0);

    id_type myID;
    hlt::Players players;
    hlt::Map map;
    hlt::getInit(map, players, myID);
    hlt::sendInit("MyC++Bot-"+std::to_string(myID));

    std::mt19937 prg(time(NULL));
    hlt::Moves moves;
    while(true) {
        moves.clear();

        hlt::getFrame(players);

        for(auto &[loc, _] : players[myID].entities) {
            moves[loc] = hlt::DIRECTIONS[prg() % 5];
        }

        hlt::sendFrame(moves);
    }

    return 0;

}
