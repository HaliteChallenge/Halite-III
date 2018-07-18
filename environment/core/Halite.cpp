#include <future>
#include <sstream>

#include "Halite.hpp"
#include "HaliteImpl.hpp"
#include "Snapshot.hpp"
#include "Logging.hpp"

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

void Halite::kill_player(const Player::id_type& player_id) {
    Logging::log("Killing player " + to_string(player_id));
    auto& player = store.get_player(player_id);
    player.crashed = true;

}

const Player& Halite::get_player(Player::id_type player_id) {
    return store.get_player(player_id);
}

// TODO: move this to parser too
constexpr auto SNAPSHOT_FIELD_DELIMITER = ";";
constexpr auto SNAPSHOT_LIST_DELIMITER = ",";
constexpr auto SNAPSHOT_SUBFIELD_DELIMITER = "-";

std::string Halite::to_snapshot() {
    std::stringstream output;

    output << HALITE_VERSION << SNAPSHOT_FIELD_DELIMITER;

    // TODO:

    return output.str();
}

/** Default destructor is defined where HaliteImpl is complete. */
Halite::~Halite() = default;

}
