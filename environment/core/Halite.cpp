#include <future>

#include "Constants.hpp"
#include "Halite.hpp"
#include "HaliteImpl.hpp"
#include "Logging.hpp"

namespace hlt {

/** Run the game. */
void Halite::run_game() {
    const auto &constants = Constants::get();
    std::unordered_map<Player::id_type, std::future<void>> results{};
    for (auto &[player_id, player] : store.players) {
        results[player_id] = std::async(std::launch::async,
                                        [&networking = networking, &player = player] {
                                            networking.initialize_player(player);
                                        });
    }
    for (auto &[_, result] : results) {
        result.get();
    }

    replay.players.insert(store.players.begin(), store.players.end());
    Logging::log("Player initialization complete.");

    for (this->turn_number = 0; this->turn_number < constants.MAX_TURNS; this->turn_number++) {
        Logging::log("Starting turn " + std::to_string(this->turn_number));
        // Create new turn struct for replay file, to be filled by further turn actions
        replay.full_frames.emplace_back();
        // Add state of entities at start of turn
        replay.full_frames.back().add_entities(store);
        impl->retrieve_commands();
        impl->process_commands();
        impl->process_production();

        if (impl->game_ended()) {
            break;
        }
    }
    game_statistics.number_turns = this->turn_number;

    impl->rank_players();
    Logging::log("Game has ended after " + std::to_string(turn_number) + " turns.");
}

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

/** Default destructor is defined where HaliteImpl is complete. */
Halite::~Halite() = default;

}
