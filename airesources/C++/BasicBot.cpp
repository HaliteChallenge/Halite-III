#include "hlt.hpp"

#include <random> // std::mt19937
#include <algorithm> // std::count_if
#include <unordered_set> // Occupied sites
#include <list> // Coordinator queues (can't use <queue> since need to inspect through moves)

/* Slight improvements:
 - Ships now move slightly more intentionally to higher halite sites for mining.
 - We're a bit better at not crashing ships into each other, via coordinated_* wrappers.
     We can still (rarely) crash into enemy ships though, if we move simultaneously.
 - Ships navigate back to the factory when full
*/

int main() {
    // Get the initial game state
    id_type myID;
    hlt::Players players;
    hlt::Map map;
    hlt::getInit(map, players, myID);

    // Respond with our name.
    hlt::sendInit("BasicC++Bot-"+std::to_string(myID));

    // Initialize the pseudorandom generator.
    std::mt19937 prg(time(NULL));

    // Coordinator to prevent collisions.
    class Coordinator {
    private:
        std::unordered_set<hlt::Location> occupied; // Sites we can't go to because they're occupied.
        std::list<id_type> extract_queue;
        std::list<std::pair<id_type, halite_type>> dump_queue;
        std::list<std::pair<id_type, hlt::Direction>> move_queue;
        halite_type spawn_request = -1;
    public:
        // Q: What about that shadowy place?
        // A: That's beyond our borders. You must never go there, Simba.
        Coordinator(const std::unordered_set<hlt::Location> & banned): occupied(banned) {}
        void extract(const id_type & id) {
            extract_queue.push_back(id);
        }
        void dump(const id_type & id, const halite_type & halite) {
            dump_queue.emplace_back(id, halite);
        }
        void move(const id_type & id, const hlt::Direction & dir) {
            move_queue.emplace_back(id, dir);
        }
        void spawn(const halite_type & halite) {
            spawn_request = halite;
        }
        void register_commands(hlt::Map & map, hlt::Player & me) {
            // Register dump and move commands first
            while(!extract_queue.empty()) {
                id_type id = extract_queue.front(); extract_queue.pop_front();
                occupied.insert(me.ships[id].location);
            }
            while(!dump_queue.empty()) {
                auto [id, halite] = dump_queue.front(); dump_queue.pop_front();
                hlt::dump(id, halite);
                occupied.insert(me.ships[id].location);
            }
            // Next priority is spawn
            if(spawn_request >= 0 && !occupied.count(me.shipyard) && me.halite >= hlt::MAX_HALITE+spawn_request) {
                hlt::spawn(spawn_request);
                occupied.insert(me.shipyard);
            }
            // First see if any moves conflict with known occupied sites. If so, cancel them.
            // Then, make one move. Repeat.
            while(!move_queue.empty()) {
                auto iter = move_queue.begin();
                while(iter != move_queue.end()) {
                    if(occupied.count(map.getLocation(me.ships[iter->first].location, iter->second))) {
                        occupied.insert(me.ships[iter->first].location);
                        iter = move_queue.erase(iter);
                    }
                    else iter++;
                }
                auto [id, dir] = move_queue.front(); move_queue.pop_front();
                hlt::move(id, dir);
                occupied.insert(map.getLocation(me.ships[id].location, dir));
            }
        }
    };

    while(true) {
        // Get the newest frame.
        unsigned int turn_number = hlt::getFrame(map, players);

        // Clear occupied set
        std::unordered_set<hlt::Location> already_occupied;
        // Add in enemy ships. Assume none are moving.
        for(auto &[player_id, player] : players) if(player_id != myID) {
            for(auto &[id, ship] : player.ships) {
                already_occupied.insert(ship.location);
            }
        }
        Coordinator coordinator(already_occupied);

        // For each of our ships
        for(auto &[id, ship] : players[myID].ships) {
            // If we have enough halite and we're on a dropoff, dump it.
            if(ship.halite > hlt::MAX_HALITE / 4 &&
              (ship.location == players[myID].shipyard || 
              std::count_if(players[myID].dropoffs.begin(), players[myID].dropoffs.end(), // Yes this is extra
              [&](const std::pair<id_type, hlt::Dropoff> & dropoff) { return ship.location == dropoff.second.location; }))) {
                coordinator.dump(id, ship.halite);
            }
            // Otherwise, if we are (nearly) full of halite, navigate towards the nearest factory or dropoff.
            else if(hlt::MAX_HALITE-ship.halite < map[ship.location]/10) {
                // Find the nearest factory / dropoff
                hlt::Location nearest_dropoff = players[myID].shipyard; // Assume it's the shipyard to start
                long nearest_distance = map.distance(ship.location, nearest_dropoff); // Calculate distance
                for(auto &[dropoff_id, dropoff] : players[myID].dropoffs) { // Try all of the dropoffs
                    long distance = map.distance(ship.location, dropoff.location); // Calculate distance
                    if(distance <= nearest_distance) { // If closer, make it the new closest.
                        nearest_dropoff = dropoff.location;
                        nearest_distance = distance;
                    }
                }
                if(ship.location.y != nearest_dropoff.y) { // Navigate N/S first
                    long y_dist = nearest_dropoff.y - ship.location.y; // Is it above or below us?
                    if(map.height - std::abs(y_dist) < std::abs(y_dist)) y_dist = -y_dist; // Is it faster to go around the map?
                    coordinator.move(id, y_dist < 0 ? hlt::Direction::NORTH : hlt::Direction::SOUTH);
                }
                else { // Then E/W
                    long x_dist = nearest_dropoff.x - ship.location.x; // Is it to the left or right of us?
                    if(map.width - std::abs(x_dist) < std::abs(x_dist)) x_dist = -x_dist; // Is it faster to go around the map?
                    coordinator.move(id, x_dist < 0 ? hlt::Direction::EAST : hlt::Direction::WEST);
                }
            }
            // If an adjacent site has more than 20% more halite than the present site, go there.
            // (Go to the best one)
            else {
                halite_type baseline = map[ship.location], best = baseline;
                hlt::Direction bestdir;
                for(auto direction : hlt::CARDINALS) { // Try all directions
                    hlt::Location newLoc = map.getLocation(ship.location, direction);
                    if(map[newLoc] >= 1.2*baseline && map[newLoc] > best) { // If more there
                        best = map[newLoc];
                        bestdir = direction; // Prefer going there
                    }
                }
                if(best != baseline) coordinator.move(id, bestdir); // Move in the preferred direction if one found
                else coordinator.extract(id); // Do nothing, which is to say, extract.
            }
        }

        // If we're in the first 200 turns and have enough halite, spawn a ship.
        if(turn_number <= 200 && players[myID].halite >= hlt::SHIP_COST) {
            coordinator.spawn(0); // We don't want to put any Halite in the newly spawned ship.
        }

        // Run coordinater to register our commands.
        coordinator.register_commands(map, players[myID]);

        // Send our moves back to the game environment
        hlt::sendFrame();
    }

    return 0;
}