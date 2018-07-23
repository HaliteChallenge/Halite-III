#include <future>

#include "HaliteImpl.hpp"
#include "Logging.hpp"

namespace hlt {

/**
 * Initialize the game.
 * @param player_commands The list of player commands.
 */
void HaliteImpl::initialize_game(const std::vector<std::string> &player_commands,
                                     const Snapshot& snapshot) {
    const auto &constants = Constants::get();
    auto &players = game.store.players;
    players.reserve(player_commands.size());
    assert(game.map.factories.size() >= player_commands.size());
    auto factory_iterator = game.map.factories.begin();
    for (const auto &command : player_commands) {
        auto &factory = *factory_iterator++;
        auto player = game.store.player_factory.make(factory, command);
        player.energy = constants.INITIAL_ENERGY;
        game.game_statistics.player_statistics.emplace_back(player.id);
        players.emplace(player.id, player);
    }
    game.replay.game_statistics = game.game_statistics;

    // Zero the energy on factories and mark as owned.
    for (auto &[player_id, player] : game.store.players) {
        auto &factory = game.map.at(player.factory);
        factory.energy = 0;
        factory.owner = player_id;
    }
}

/** Run the game. */
void HaliteImpl::run_game() {
    const auto &constants = Constants::get();

    id_map<Player, std::future<void>> results{};
    for (auto &[player_id, player] : game.store.players) {
        results[player_id] = std::async(std::launch::async,
                                        [&networking = game.networking, &player = player] {
                                            networking.initialize_player(player);
                                        });
    }
    for (auto &[player_id, result] : results) {
        game.store.players.at(player_id).log_error_section("Initialization Phase");
        try {
            result.get();
        }
        catch (const BotError& e) {
            game.kill_player(player_id);
        }
    }
    game.replay.players.insert(game.store.players.begin(), game.store.players.end());
    Logging::log("Player initialization complete.");

    for (game.turn_number = 0; game.turn_number < constants.MAX_TURNS; game.turn_number++) {
        Logging::log("Starting turn " + std::to_string(game.turn_number));
        // Create new turn struct for replay file, to be filled by further turn actions
        game.replay.full_frames.emplace_back();
        // Add state of entities at start of turn
        game.replay.full_frames.back().add_entities(game.store);
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
        if (player.is_alive()) {
            results[player_id] = std::async(std::launch::async,
                                            [&game = this->game, &player = player] {
                                                return game.networking.handle_frame(player);
                                            });
        }
    }
    for (auto &[player_id, result] : results) {
        game.store.players.at(player_id).log_error_section("Turn " + std::to_string(game.turn_number));
        try {
            commands[player_id] = result.get();
        }
        catch (const BotError& e) {
            game.kill_player(player_id);
        }
    }

    // Process valid player commands, removing players if they submit invalid ones.
    std::unordered_set<Entity::id_type> changed_entities;
    while (!commands.empty()) {
        CommandTransaction transaction{game.store, game.map};
        transaction.set_callback([&frames = game.replay.full_frames](auto event) {
            // Create new game event for replay file.
            frames.back().events.push_back(std::move(event));
        });
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
            game.store.changed_cells = std::move(transaction.changed_cells);
            // add player commands to replay and note players still alive
            game.replay.full_frames.back().moves = std::move(commands);
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
    const auto max_energy = Constants::get().MAX_ENERGY;
    for (auto &[entity_id, entity] : game.store.entities) {
        if (changed_entities.find(entity_id) == changed_entities.end()
            && entity.energy < max_energy) {
            // Allow this entity to extract
            const auto location = game.store.get_player(entity.owner).get_entity_location(entity_id);
            auto &cell = game.map.at(location);
            energy_type extracted = cell.energy / ratio;
            // If energy is small, give it all to the entity.
            if (extracted == 0 && cell.energy > 0) {
                extracted = cell.energy;
            }
            // Do not allow entity to exceed capacity.
            if (max_energy - entity.energy < extracted) {
                extracted = max_energy - entity.energy;
            }
            entity.energy += extracted;
            cell.energy -= extracted;
            game.store.changed_cells.emplace(location);
        }
    }
    game.replay.full_frames.back().add_cells(game.map, game.store.changed_cells);
    for (const auto [player_id, player] : game.store.players) {
        game.replay.full_frames.back().energy.insert( {{player_id, player.energy}} );
    }
    update_player_stats();
}

/**
 * Determine whether the game has ended.
 * @return True if the game has ended.
 */
bool HaliteImpl::game_ended() const {
    long num_alive_players = 0;
    for (auto &&[_, player] : game.store.players) {
        // TODO: check that this is correct
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
 * Update a player's statistics after a single turn. This will update their current game production and their last turn
 * alive if they are still alive.
 *
 * @param productions Mapping from player ID to the production they gained in the current turn.
 */
void HaliteImpl::update_player_stats() {
    for (PlayerStatistics &player_stats : game.game_statistics.player_statistics) {
        // Player with sprites is still alive, so mark as alive on this turn and add production gained
        const auto& player_id = player_stats.player_id;
        if (game.store.get_player(player_id).is_alive()) {
            player_stats.last_turn_alive = game.turn_number;
            player_stats.turn_productions.push_back(game.store.get_player(player_id).energy);
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
