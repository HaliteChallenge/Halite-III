#include <future>

#include "BasicGenerator.hpp"
#include "Command.hpp"
#include "Constants.hpp"
#include "Generator.hpp"
#include "Halite.hpp"
#include "HaliteImpl.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Location.hpp"
#include "Statistics.hpp"
#include "Replay.hpp"
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
               const std::vector<std::string> &player_commands,
               GameStatistics &game_statistics,
               Replay &replay) :
        map(map),
        game_statistics(game_statistics),
        replay(replay),
        config(config),
        networking(networking_config, *this),
        impl(std::make_unique<HaliteImpl>(*this)) {
    const auto &constants = Constants::get();
    auto &players = store.players;
    players.reserve(player_commands.size());
    assert(map.factories.size() >= player_commands.size());
    auto factory_iterator = map.factories.begin();
    for (const auto &command : player_commands) {
        auto &factory = *factory_iterator++;
        auto player = store.player_factory.make(factory, command);
        player.energy = constants.INITIAL_ENERGY;
        game_statistics.player_statistics.emplace_back(player.id);
        players.emplace(player.id, player);
    }
    replay.game_statistics = game_statistics;
}

void Halite::load_snapshot(const Snapshot &snapshot) {
    // TODO: implement
    (void) snapshot;
}

/** Run the game. */
void Halite::run_game() {
    impl->run_game();
}

/** Default destructor is defined where HaliteImpl is complete. */
Halite::~Halite() = default;

}
