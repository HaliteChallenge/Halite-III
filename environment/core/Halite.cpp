#include <future>

#include "Halite.hpp"
#include "HaliteImpl.hpp"
#include "Snapshot.hpp"

namespace hlt {

/**
 * Constructor for the main game.
 *
 * @param config The configuration options for the game.
 * @param map The game map.
 * @param networking_config The networking configuration.
 * @param players The list of players.
 * @param game_statistics The game statistics to use.
 * @param replay The game replay to use.
 */
Halite::Halite(const Config &config,
               Map &map,
               const net::NetworkingConfig &networking_config,
               GameStatistics &game_statistics,
               Replay &replay) :
        map(map),
        game_statistics(game_statistics),
        replay(replay),
        config(config),
        networking(networking_config, *this),
        impl(std::make_unique<HaliteImpl>(*this)) {
}

void Halite::load_snapshot(const Snapshot &snapshot) {
    // TODO: implement
    (void) snapshot;
}

/**
 * Run the game.
 * @param player_commands The list of player commands.
 */
void Halite::run_game(const std::vector<std::string> &player_commands) {
    impl->initialize_game(player_commands);
    impl->run_game();
}

/** Default destructor is defined where HaliteImpl is complete. */
Halite::~Halite() = default;

}
