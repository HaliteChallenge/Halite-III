#include "Constants.hpp"
#include "BlurTileGenerator.hpp"
#include "Halite.hpp"
#include "HaliteImpl.hpp"
#include "Logging.hpp"

namespace hlt {
    std::unordered_map<hlt::id_type , hlt::Command> generate_move_list(std::unordered_map<hlt::id_type , hlt::Player> &players) {
        std::unordered_map<hlt::id_type , hlt::Command> moves;
        for (std::pair<const hlt::id_type, hlt::Player> &player_pair : players) {
            for (const auto entity_pair : player_pair.second.entities) {
                hlt::Location location = entity_pair.first;
                moves[player_pair.first] = std::make_unique<hlt::MoveCommand>(location.first, location.second, hlt::Direction::North);
            }
        }
        return moves;
    };

/** Run the game. */
void Halite::run_game() {
//    for (auto &player : players) {
//        networking.initialize_player(player.second);
//    }
    const auto &constants = Constants::get();
    for (this->turn_number = 0; this->turn_number < constants.MAX_TURNS; this->turn_number++) {
        // Create new turn struct for replay file, to be filled by further turn actions
        replay_struct.full_frames.emplace_back();
        impl->process_commands(generate_move_list(this->players));
        impl->process_production();
        impl->process_entities();

        if (impl->game_ended()) {
            break;
        }
    }

    impl->rank_players();
    Logging::log("Game has ended after " + std::to_string(turn_number) + " turns.");
    // TODO: generate replay
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
        config(config),
        parameters(parameters),
        networking(net::Networking(networking_config, this)),
        impl(std::make_unique<HaliteImpl>(this)),
        game_map(mapgen::BlurTileGenerator(parameters).generate(players)),
        replay_struct(this->game_statistics, parameters.num_players, players, parameters.seed, this->game_map) {
    for (const auto &player : players) {
        this->players[player.player_id] = player;
        game_statistics.player_statistics.emplace_back(player.player_id);
    }
    replay_struct.game_statistics = game_statistics;
}

/** Default destructor is defined where HaliteImpl is complete. */
Halite::~Halite() = default;

}
