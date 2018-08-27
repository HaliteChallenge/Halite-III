#include <future>
#include <sstream>

#include "Halite.hpp"
#include "HaliteImpl.hpp"

namespace hlt {

/**
 * Constructor for the main game.
 *
 * @param map The game map.
 * @param networking_config The networking configuration.
 * @param players The list of players.
 * @param game_statistics The game statistics to use.
 * @param replay The game replay to use.
 */
Halite::Halite(Map &map,
               const net::NetworkingConfig &networking_config,
               GameStatistics &game_statistics,
               Replay &replay) :
        map(map),
        game_statistics(game_statistics),
        replay(replay),
        networking(networking_config, *this),
        impl(std::make_unique<HaliteImpl>(*this)),
        rng(replay.map_generator_seed) {}

/**
 * Run the game.
 * @param player_commands The list of player commands.
 */
void Halite::run_game(const std::vector<std::string> &player_commands,
                      const Snapshot &snapshot) {
    impl->initialize_game(player_commands, snapshot);
    impl->run_game();
}

std::string Halite::to_snapshot(const hlt::mapgen::MapParameters &map_parameters) {
    std::stringstream output;

    output << HALITE_VERSION << SNAPSHOT_FIELD_DELIMITER;

    output << map_parameters.type
           << SNAPSHOT_LIST_DELIMITER << map_parameters.width
           << SNAPSHOT_LIST_DELIMITER << map_parameters.height
           << SNAPSHOT_LIST_DELIMITER << map_parameters.num_players
           << SNAPSHOT_LIST_DELIMITER << map_parameters.seed
           << SNAPSHOT_FIELD_DELIMITER;

    for (const auto &row : map.grid) {
        for (const auto &cell : row) {
            output << cell.energy << SNAPSHOT_LIST_DELIMITER;
        }
    }
    output << SNAPSHOT_FIELD_DELIMITER;

    for (const auto&[player_id, player] : store.players) {
        output << player_id
               << SNAPSHOT_FIELD_DELIMITER << player.energy
               << SNAPSHOT_FIELD_DELIMITER
               << player.factory.x << SNAPSHOT_SUBFIELD_DELIMITER
               << player.factory.y << SNAPSHOT_LIST_DELIMITER;

        for (const auto &dropoff : player.dropoffs) {
            output << dropoff.id << SNAPSHOT_SUBFIELD_DELIMITER
                   << dropoff.location.x << SNAPSHOT_SUBFIELD_DELIMITER
                   << dropoff.location.y << SNAPSHOT_LIST_DELIMITER;
        }

        output << SNAPSHOT_FIELD_DELIMITER;

        for (const auto&[entity_id, entity_location] : player.entities) {
            const auto &entity = store.entities.at(entity_id);
            output << entity_id << SNAPSHOT_SUBFIELD_DELIMITER
                   << entity_location.x << SNAPSHOT_SUBFIELD_DELIMITER
                   << entity_location.y << SNAPSHOT_SUBFIELD_DELIMITER
                   << entity.energy << SNAPSHOT_LIST_DELIMITER;
        }

        output << SNAPSHOT_FIELD_DELIMITER;
    }

    return output.str();
}

/** Default destructor is defined where HaliteImpl is complete. */
Halite::~Halite() = default;

}
