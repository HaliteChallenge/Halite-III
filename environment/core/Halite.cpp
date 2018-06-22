#include <future>

#include "Constants.hpp"
#include "BlurTileGenerator.hpp"
#include "Halite.hpp"
#include "HaliteImpl.hpp"
#include "Logging.hpp"

namespace hlt {

/** Run the game. */
void Halite::run_game() {
    std::unordered_map<Player::id_type, std::future<void>> results;
    for (auto &[player_id, player] : players) {
        results[player_id] = std::async(std::launch::async,
                                        [&networking = networking, &player = player] {
                                            networking.initialize_player(player);
                                        });
    }
    for (auto &[_, result] : results) {
        result.wait();
    }

    const auto &constants = Constants::get();
    for (this->turn_number = 0; this->turn_number < constants.MAX_TURNS; this->turn_number++) {
        Logging::log("Starting turn " + std::to_string(this->turn_number));
        impl->retrieve_commands();
        impl->process_commands();
        impl->process_production();
        impl->process_entities();

        if (impl->game_ended()) {
            break;
        }
    }

    impl->rank_players();
    Logging::log("Game has ended after " + std::to_string(turn_number) + " turns.");
    // TODO: generate replay
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
        game_map(mapgen::BlurTileGenerator(parameters).generate(players)),
        config(config),
        parameters(parameters),
        networking(net::Networking(networking_config, this)),
        impl(std::make_unique<HaliteImpl>(this)) {
    for (const auto &player : players) {
        this->players[player.player_id] = player;
        game_stats.player_statistics.emplace_back(player.player_id);
    }
}

/** Default destructor is defined where HaliteImpl is complete. */
Halite::~Halite() = default;

}
