#include "BasicGenerator.hpp"
#include "Halite.hpp"

namespace hlt {

void Halite::retrieve_moves() {
    // TODO: implement
}

void Halite::process_commands() {
    // TODO: implement
}

void Halite::process_production() {
    // TODO: implement
}

void Halite::run_game() {
    for (auto &player : players) {
        networking.initialize_player(player);
    }
    // TODO: start main event loop
    // TODO: thread the communications with players
}

Halite::Halite(const Config &config, const mapgen::MapParameters &parameters,
               const net::NetworkingConfig &networking_config, std::list<Player> players) :
        config(config), parameters(parameters), networking(net::Networking(networking_config, this)), players(players),
        game_map(mapgen::BasicGenerator(parameters).generate(players)) {}

}
