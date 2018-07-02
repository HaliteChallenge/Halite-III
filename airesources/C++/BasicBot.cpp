#include "hlt.hpp"

#include <random>

int main() {
    std::cout.sync_with_stdio(0);
    Logging::open("log.log");

    id_type myID;
    hlt::Players players;
    hlt::Map map;
    hlt::getInit(map, players, myID);
    hlt::sendInit("BasicC++Bot-"+std::to_string(myID));

    Logging::log("myID: "+std::to_string(myID));

    std::mt19937 prg(time(NULL));
    hlt::Moves moves;
    while(true) {
        moves.clear();

        hlt::getFrame(players);

        // I'm so, so sorry about how inefficient this is. Coding fast ¯\_(ツ)_/¯
        std::vector< std::vector<double> > values(map.height, std::vector<double>(map.width, 0));
        for(long y = 0; y < map.height; y++) for(long x = 0; x < map.width; x++) {
            for(auto &[id, player] : players) {
                double playerWeight = 0;
                playerWeight += 2.0 / (map.distance({ x, y }, player.factory_location) + 1);
                //Logging::log(std::to_string(playerWeight));
                for(auto &[loc, _] : player.entities) {
                    playerWeight += 1.0 / (map.distance({ x, y }, loc) + 1);
                }
                values[y][x] += id != myID ? playerWeight : -playerWeight;
            }
        }

        for(auto &[loc, _] : players[myID].entities) {
            double max_val = values[loc.y][loc.x];
            hlt::Direction bestDir = hlt::Direction::STILL;
            for(auto &d : hlt::CARDINALS) {
                hlt::Location l = map.getLocation(loc, d);
                if(values[l.y][l.x] > max_val) {
                    bestDir = d;
                    max_val = values[l.y][l.x];
                }
            }
            moves[loc] = bestDir;
        }

        hlt::sendFrame(moves);
    }

    return 0;

}
