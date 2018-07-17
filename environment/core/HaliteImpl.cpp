#include <future>

#include "HaliteImpl.hpp"
#include "Logging.hpp"

namespace hlt {

/** Run the game. */
void HaliteImpl::run_game() {
    const auto &constants = Constants::get();

    // Zero the energy on factories.
    for (auto &[_, player] : game.store.players) {
        game.map.at(player.factory).energy = 0;
    }

    id_map<Player, std::future<void>> results{};
    for (auto &[player_id, player] : game.store.players) {
        results[player_id] = std::async(std::launch::async,
                                        [&networking = game.networking, &player = player] {
                                            networking.initialize_player(player);
                                        });
    }
    for (auto &[_, result] : results) {
        result.get();
    }
    game.replay.players.insert(game.store.players.begin(), game.store.players.end());
    game.replay.full_frames.emplace_back();
    Logging::log("Player initialization complete.");

    for (game.turn_number = 0; game.turn_number < constants.MAX_TURNS; game.turn_number++) {
        Logging::log("Starting turn " + std::to_string(game.turn_number));
        // Create new turn struct for replay file, to be filled by further turn actions
        game.replay.full_frames.emplace_back();
        process_turn();

        if (game_ended()) {
            break;
        }
    }
    game.game_statistics.number_turns = game.turn_number;

    rank_players();
    Logging::log("Game has ended after " + std::to_string(game.turn_number) + " turns.");
}

/** Retrieve and process commands, and update the game state for the current turn. */
void HaliteImpl::process_turn() {
    using Commands = std::vector<std::unique_ptr<Command>>;
    id_map<Player, Commands> commands{};
    id_map<Player, std::future<Commands>> results{};
    for (auto &[player_id, player] : game.store.players) {
        results[player_id] = std::async(std::launch::async,
                                        [&game = this->game, &player = player] {
                                            return game.networking.handle_frame(player);
                                        });
    }
    for (auto &[player_id, result] : results) {
        commands[player_id] = result.get();
    }

    // Process valid player commands, removing players if they submit invalid ones.
    std::unordered_set<Entity::id_type> changed_entities;
    std::unordered_set<Location> changed_cells;
    while (!commands.empty()) {
        CommandTransaction transaction{game.store, game.map};
        for (const auto &[player_id, command_list] : commands) {
            auto &player = game.store.players.find(player_id)->second;
            for (const auto &command : command_list) {
                command->add_to_transaction(player, transaction);
            }
        }
        if (transaction.check()) {
            // All commands are successful.
            transaction.commit();
            changed_entities = std::move(transaction.changed_entities);
            changed_cells = std::move(transaction.changed_cells);
            break;
        } else {
            for (auto &offender : transaction.offenders) {
                // Remove this player and their commands.
                for (auto command_iterator = commands.begin(); command_iterator != commands.end();) {
                    if (command_iterator->first == offender) {
                        command_iterator = commands.erase(command_iterator);
                    } else {
                        command_iterator++;
                    }
                }
                // TODO: kill the player over the network and mark them dead
            }
        }
    }

    const auto ratio = Constants::get().EXTRACT_RATIO;
    for (auto &[entity_id, entity] : game.store.entities) {
        if (changed_entities.find(entity_id) == changed_entities.end()) {
            // Allow this entity to extract
            const auto location = game.store.get_player(entity.owner).get_entity_location(entity_id);
            auto &cell = game.map.at(location);
            auto extracted = cell.energy / ratio;
            // TODO: edge case here when the energy is small
            // TODO: maximum capacity of ship
            entity.energy += extracted;
            cell.energy -= extracted;
            changed_cells.emplace(location);
        }
    }
    // TODO: replay now has access to changed_entities and changed_cells
}

/**
 * Determine whether the game has ended.
 * @return True if the game has ended.
 */
bool HaliteImpl::game_ended() const {
    long num_alive_players = 0;
    for (auto &&[_, player] : game.store.players) {
        // TODO: implement edge case of last player being unable to produce
        if (player.is_alive()) {
            num_alive_players++;
        }
        if (num_alive_players > 1) {
            return false;
        }
    }
    return true;
}

/**
 * Update a player's statistics after a single turn. This will update their total game production, their last turn
 * alive if they are still alive, and the production for that turn.
 *
 * @param productions Mapping from player ID to the production they gained in the current turn.
 */
void HaliteImpl::update_player_stats(std::unordered_map<Player::id_type, energy_type> &productions) {
    for (PlayerStatistics &player_stats : game.game_statistics.player_statistics) {
        // Player with sprites is still alive, so mark as alive on this turn and add production gained
        if (game.store.get_player(player_stats.player_id).is_alive()) {
            player_stats.last_turn_alive = game.turn_number;
            player_stats.turn_productions.push_back(productions[player_stats.player_id]);
            player_stats.total_production += productions[player_stats.player_id];
        } else {
            player_stats.turn_productions.push_back(0);
        }
    }
}

/**
 * Update players' rankings based on their final turn alive, then break ties with production totals in final turn.
 * Function is intended to be called at end of game, and will in place modify the ranking field of player statistics
 * to rank players from winner (1) to last player.
 */
void HaliteImpl::rank_players() {
    auto &statistics = game.game_statistics.player_statistics;
    std::stable_sort(statistics.begin(), statistics.end());
    // Reverse list to have best players first
    std::reverse(statistics.begin(), statistics.end());

    for (size_t index = 0; index < statistics.size(); ++index) {
        statistics[index].rank = index + 1l;
    }
}

/**
 * Construct HaliteImpl from game interface.
 * @param game The game interface.
 */
HaliteImpl::HaliteImpl(Halite &game) : game(game) {}

}
