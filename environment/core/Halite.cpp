#include "Constants.hpp"
#include "BasicGenerator.hpp"
#include "Halite.hpp"

namespace hlt {

/** Communicate with bots to obtain moves for next step. */
void Halite::retrieve_moves() {
    // TODO: implement
}

/** Process all commands for this turn. */
void Halite::process_commands() {
    // TODO: implement
}

/** Process all production results for this turn. */
void Halite::process_production() {
    // TODO: implement
}

/** Run the game. */
void Halite::run_game() {
    for (auto &player : players) {
        networking.initialize_player(player);
    }
    const auto &constants = GameConstants::get();
    for (turn_number = 0; turn_number < constants.MAX_TURNS; turn_number++) {
        for (auto &player : players) {
            Command command;
            networking.handle_frame(player, command);
        }
    }
    // TODO: start main event loop
    // TODO: thread the communications with players
}

/**
 * Constructor for the main game.
 *
 * @param config The configuration options for the game.
 * @param parameters The map generation parameters.
 * @param networking_config The networking configuration.
 * @param players The list of players.
 */
Halite::Halite(const Config &config,
               const mapgen::MapParameters &parameters,
               const net::NetworkingConfig &networking_config,
               std::list<Player> players) :
        config(config), parameters(parameters),
        networking(net::Networking(networking_config, this)), players(players),
        game_map(mapgen::BasicGenerator(parameters).generate(players)) {}

}
