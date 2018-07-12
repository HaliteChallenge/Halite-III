#include <future>

#include "Constants.hpp"
#include "BlurTileGenerator.hpp"
#include "FractalValueNoiseTileGenerator.hpp"
#include "Halite.hpp"
#include "HaliteImpl.hpp"
#include "Logging.hpp"

namespace hlt {

/** Run the game. */
void Halite::run_game() {
    const auto &constants = Constants::get();
    std::unordered_map<Player::id_type, std::future<void>> results;
    for (auto &[player_id, player] : players) {
        results[player_id] = std::async(std::launch::async,
                                        [&networking = networking, &player = player] {
                                            networking.initialize_player(player);
                                        });
    }
    for (auto &[_, result] : results) {
        result.get();
    }
    replay.players = this->players;
    replay.full_frames.emplace_back();
    Logging::log("Player initialization complete.");

    for (this->turn_number = 0; this->turn_number < constants.MAX_TURNS; this->turn_number++) {
        Logging::log("Starting turn " + std::to_string(this->turn_number));
        // Create new turn struct for replay file, to be filled by further turn actions
        replay.full_frames.emplace_back();
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
 * @param parameters The map generation parameters.
 * @param networking_config The networking configuration.
 * @param players The list of players.
 */
Halite::Halite(const Config &config,
               const mapgen::MapParameters &parameters,
               const net::NetworkingConfig &networking_config,
               std::vector<Player> players) :
        game_map(parameters.width, parameters.height),
        replay(this->game_statistics, parameters.num_players, parameters.seed, this->game_map),
        config(config),
        parameters(parameters),
        networking(net::Networking(networking_config, *this)),
        impl(std::make_unique<HaliteImpl>(*this)) {
    std::unique_ptr<mapgen::Generator> generator;
    switch (parameters.type) {
    case mapgen::MapType::Basic:
        generator = std::make_unique<mapgen::BasicGenerator>(parameters);
        break;
    case mapgen::MapType::BlurTile:
        generator = std::make_unique<mapgen::BlurTileGenerator>(parameters);
        break;
    case mapgen::MapType::Fractal:
        generator = std::make_unique<mapgen::FractalValueNoiseTileGenerator>(parameters);
        break;
    }
    game_map.map_generator = generator->name();
    std::vector<Location> factories;
    factories.reserve(players.size());
    generator->generate(game_map, factories);

    const auto &constants = Constants::get();
    for (const auto &player : players) {
        this->players[player.player_id] = player;
        this->players[player.player_id].factory = factories.back();
        factories.pop_back();
        this->players[player.player_id].energy = constants.INITIAL_ENERGY;
        game_statistics.player_statistics.emplace_back(player.player_id);
    }

    replay.game_statistics = game_statistics;
}

void Halite::load_snapshot(const Snapshot& snapshot) {
    // TODO: implement
    (void)snapshot;
}

/** Default destructor is defined where HaliteImpl is complete. */
Halite::~Halite() = default;

}
