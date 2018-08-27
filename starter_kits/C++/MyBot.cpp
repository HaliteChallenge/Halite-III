#include "hlt/hlt.hpp"

#include <random>

int main() {
    hlt::Game game{};
    // Get the inital game state
    game.get_init();

    // Initialize the pseudorandom generator.
    std::mt19937 prg(time(NULL));

    // Respond with our name
    game.send_init("MyC++Bot-" + std::to_string(game.my_id));

    while (true) {
        // Get the newest frame.
        const auto turn_number = game.get_frame();

        const auto me = game.players.at(game.my_id);
        std::vector<std::unique_ptr<hlt::Command>> command_queue;
        hlt::SafeMover safe_mover{game.game_map, game.players};

        auto at_home = false;
        // For each of our ships
        for (const auto &id_ship : game.players[game.my_id].ships) {
            const auto id = id_ship.first;
            const auto &ship = id_ship.second;
            // If we're on our shipyard and have enough halite, remember this.
            if (ship.location == game.me().shipyard.location) {
                at_home = true;
                // Halite gets collected automatically when we move
                // over the shipyard.
            }

            // Otherwise, check if there's a reasonable amount of halite on the square and we have capacity.
            // If so, extract halite. If not, move randomly.
            if (game.game_map[ship.location] > hlt::MAX_HALITE / 10 &&
                     !ship.is_full()) {
                continue; // Do nothing, which is to say, extract halite.
            }
            else {
                // Move in a random direction
                command_queue.push_back(safe_mover.move(ship, hlt::CARDINALS[prg() % 4]));
            }
        }

        // If we're in the first 200 turns and have enough halite, spawn a ship.
        if (turn_number <= 200 &&
            game.players[game.my_id].halite >= hlt::SHIP_COST &&
            !at_home) {
            command_queue.push_back(game.me().shipyard.spawn());
        }

        // Send our moves back to the game environment
        game.end_turn(command_queue);
    }

    return 0;
}
