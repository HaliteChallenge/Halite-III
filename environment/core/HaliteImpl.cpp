#include <future>

#include "HaliteImpl.hpp"
#include "Logging.hpp"

namespace hlt {

/**
 * Initialize the game.
 * @param player_commands The list of player commands.
 */
void HaliteImpl::initialize_game(const std::vector<std::string> &player_commands,
                                 const Snapshot &snapshot) {
    const auto &constants = Constants::get();
    auto &players = game.store.players;
    assert(game.map.factories.size() >= player_commands.size());

    // Add a 0 frame so we can record beginning-of-game state
    game.replay.full_frames.emplace_back();
    std::unordered_set<Location> changed_cells;

    auto factory_iterator = game.map.factories.begin();

    // Load the map from the snapshot
    if (snapshot.map.size() > 0) {
        assert(snapshot.map.size() == static_cast<decltype(snapshot.map)::size_type>(game.map.width * game.map.height));

        for (dimension_type row = 0; row < game.map.height; row++) {
            for (dimension_type col = 0; col < game.map.width; col++) {
                game.map.at(col, row).energy = snapshot.map.at(static_cast<dimension_type>(row * game.map.width + col));
                changed_cells.emplace(row, col);
            }
        }
    }

    for (dimension_type row = 0; row < game.map.height; row++) {
        for (dimension_type col = 0; col < game.map.width; col++) {
            game.store.map_total_energy += game.map.at(col, row).energy;
        }
    }

    for (const auto &command : player_commands) {
        auto &factory = *factory_iterator++;
        auto player = game.store.player_factory.make(factory, command);
        player.energy = constants.INITIAL_ENERGY;
        game.game_statistics.player_statistics.emplace_back(player.id);
        if (snapshot.players.find(player.id) != snapshot.players.end()) {
            const auto &player_snapshot = snapshot.players.at(player.id);
            player.factory = player_snapshot.factory;
            player.energy = player_snapshot.energy;

            for (const auto &[_, dropoff_location] : player_snapshot.dropoffs) {
                auto &cell = game.map.at(dropoff_location);
                cell.owner = player.id;
                player.dropoffs.emplace_back(game.store.new_dropoff(dropoff_location));
                game.replay.full_frames.back().events.push_back(
                        std::make_unique<ConstructionEvent>(
                                dropoff_location, player.id, Entity::id_type{0}));
                changed_cells.insert(dropoff_location);
            }

            for (const auto &entity : player_snapshot.entities) {
                auto &cell = game.map.at(entity.location);
                const auto &new_entity = game.store.new_entity(entity.energy, player.id);
                cell.entity = new_entity.id;
                player.add_entity(new_entity.id, entity.location);
                game.replay.full_frames.back().events.push_back(
                        std::make_unique<SpawnEvent>(
                                entity.location, entity.energy, player.id, cell.entity));
            }
        }
        players.emplace(player.id, player);
    }
    game.replay.full_frames.back().add_cells(game.map, changed_cells);

    game.replay.game_statistics = game.game_statistics;

    for (auto &[player_id, player] : game.store.players) {
        // Zero the energy on factory and mark as owned.
        auto &factory = game.map.at(player.factory);
        game.store.map_total_energy -= factory.energy;
        factory.energy = 0;
        factory.owner = player_id;
        // Prepare the log.
        game.logs.add(player_id);
    }
}

/** Run the game. */
void HaliteImpl::run_game() {
    const auto &constants = Constants::get();

    id_map<Player, std::future<void>> results{};
    for (auto &[player_id, player] : game.store.players) {
        Logging::log("Initializing player " + to_string(player.id) + " with command " + player.command);
        results[player_id] = std::async(std::launch::async,
                                        [&networking = game.networking, &player = player] {
                                            networking.initialize_player(player);
                                        });
    }
    for (auto &[player_id, result] : results) {
        try {
            result.get();
        }
        catch (const BotError &e) {
            kill_player(player_id);
        }
    }
    game.replay.players.insert(game.store.players.begin(), game.store.players.end());
    Logging::log("Player initialization complete.");

    for (game.turn_number = 0; game.turn_number < constants.MAX_TURNS; game.turn_number++) {
        Logging::set_turn_number(game.turn_number);
        game.logs.set_turn_number(game.turn_number);
        Logging::log([turn_number = game.turn_number]() {
            return "Starting turn " + std::to_string(turn_number);
        }, Logging::Level::Debug);
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
    Logging::set_turn_number(Logging::ended);
    game.logs.set_turn_number(PlayerLog::ended);
    Logging::log("Game has ended.");
    Logging::remove_turn_number();
    for (const auto &[player_id, player] : game.store.players) {
        game.replay.players.find(player_id)->second.terminated = player.terminated;
        if (!player.terminated) {
            game.networking.kill_player(player);
        }
    }
}

/** Retrieve and process commands, and update the game state for the current turn. */
void HaliteImpl::process_turn() {
    using Commands = std::vector<std::unique_ptr<Command>>;
    ordered_id_map<Player, Commands> commands{};
    id_map<Player, std::future<Commands>> results{};
    for (auto &[player_id, player] : game.store.players) {
        if (player.is_alive()) {
            results[player_id] = std::async(std::launch::async,
                                            [&networking = game.networking, &player = player] {
                                                return networking.handle_frame(player);
                                            });
        }
    }
    for (auto &[player_id, result] : results) {
        try {
            commands[player_id] = result.get();
        }
        catch (const BotError &e) {
            kill_player(player_id);
        }
    }

    // Process valid player commands, removing players if they submit invalid ones.
    std::unordered_set<Entity::id_type> changed_entities;
    while (!commands.empty()) {
        changed_entities.clear();
        game.store.changed_cells.clear();

        CommandTransaction transaction{game.store, game.map};
        std::unordered_set<Player::id_type> offenders;
        transaction.on_event([&frames = game.replay.full_frames](GameEvent event) {
            // Create new game event for replay file.
            frames.back().events.push_back(std::move(event));
        });
        transaction.on_error([&offenders, &commands, this](CommandError error) {
            this->handle_error(offenders, commands, std::move(error));
        });
        transaction.on_cell_update([&changed_cells = game.store.changed_cells](Location cell) {
            changed_cells.emplace(cell);
        });
        transaction.on_entity_update([&changed_entities](Entity::id_type entity) {
            changed_entities.emplace(entity);
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
            if (Constants::get().STRICT_ERRORS) {
                if (!offenders.empty()) {
                    std::ostringstream stream;
                    stream << "Command processing failed for players: ";
                    for (auto iterator = offenders.begin(); iterator != offenders.end(); iterator++) {
                        stream << *iterator;
                        if (std::next(iterator) != offenders.end()) {
                            stream << ", ";
                        }
                    }
                    stream << ". Aborting due to strict error check.";
                    Logging::log(stream.str(), Logging::Level::Error);
                    game.turn_number = Constants::get().MAX_TURNS;
                    return;
                }
            } else {
                assert(offenders.empty());
            }
            // Add player commands to replay and note players still alive
            game.replay.full_frames.back().moves = std::move(commands);
            break;
        } else {
            for (auto player : offenders) {
                kill_player(player);
                commands.erase(player);
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
            game.store.map_total_energy -= extracted;
            game.store.changed_cells.emplace(location);
        }
    }
    game.replay.full_frames.back().add_cells(game.map, game.store.changed_cells);
    for (const auto &[player_id, player] : game.store.players) {
        game.replay.full_frames.back().energy.insert({{player_id, player.energy}});
    }
    update_player_stats();
}

/**
 * Determine whether the game has ended.
 * @return True if the game has ended.
 */
bool HaliteImpl::game_ended() const {
    if (game.store.map_total_energy == 0) {
        return true;
    }
    long num_alive_players = 0;
    for (auto &&[_, player] : game.store.players) {
        bool can_play = !player.entities.empty() || player.energy > Constants::get().NEW_ENTITY_ENERGY_COST;
        if (player.is_alive() && can_play) {
            num_alive_players++;
        }
        if (num_alive_players > 1) {
            return false;
        }
    }
    return true;
}

/**
 * Update all players' statistics after a single turn.
 */
void HaliteImpl::update_player_stats() {
    for (PlayerStatistics &player_stats : game.game_statistics.player_statistics) {
        // Player with sprites is still alive, so mark as alive on this turn and add production gained
        const auto &player_id = player_stats.player_id;
        if (game.store.get_player(player_id).is_alive()) {
            const Player &player = game.store.get_player(player_id);
            player_stats.last_turn_alive = game.turn_number;
            player_stats.turn_productions.push_back(player.energy);
            player_stats.number_dropoffs = player.dropoffs.size();
            for (const auto &[_entity_id, location] : player.entities) {
                const dimension_type entity_distance = game.map.distance(location, player.factory);
                if (entity_distance > player_stats.max_entity_distance)
                    player_stats.max_entity_distance = entity_distance;
                player_stats.total_distance += entity_distance;
                player_stats.total_entity_lifespan++;
                if (possible_interaction(player_id, location)) {
                    player_stats.interaction_opportunities++;
                }
            }
        } else {
            player_stats.turn_productions.push_back(0);
        }
    }
}

/**
 * Determine if entity owned by given player is in range of another player (their entity, dropoff, or factory) and thus may interact
 *
 * param owner_id Id of owner of entity at given location
 * param entity_location Location of entity we are assessing for an interaction opportunity
 * return bool Indicator of whether there players are in close range for an interaction (true) or not (false)
 */
bool HaliteImpl::possible_interaction(const Player::id_type owner_id, const Location entity_location) {
    // Fetch all locations 2 cells away
    std::unordered_set<Location> close_cells{};
    const auto neighbors = game.map.get_neighbors(entity_location);
    close_cells.insert(neighbors.begin(), neighbors.end());
    for (const Location &neighbor : neighbors) {
        const auto cells_once_removed = game.map.get_neighbors(neighbor);
        close_cells.insert(cells_once_removed.begin(), cells_once_removed.end());
    }
    // Interaction possibilty implies a cell has an entity owned by another player or there is a factory or dropoff
    // of another player on the cell. Interactions between entities of a single player are ignored
    for (const Location &cell_location : close_cells) {
        const Cell &cell = game.map.at(cell_location);
        if (cell.entity != Entity::None) {
            if (game.store.get_entity(cell.entity).owner != owner_id) return true;
        }
        if (cell.owner != Player::None && cell.owner != owner_id) return true;
    }
    return false;

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

void HaliteImpl::kill_player(const Player::id_type &player_id) {
    Logging::log("Killing player " + to_string(player_id), Logging::Level::Warning);
    auto &player = game.store.get_player(player_id);
    player.terminated = true;
    game.networking.kill_player(player);

    auto &entities = player.entities;
    for (auto entity_iterator = entities.begin();
         entity_iterator != entities.end();
         entity_iterator = entities.erase(entity_iterator)) {
        const auto &[entity_id, location] = *entity_iterator;
        auto &cell = game.map.at(location);
        cell.entity = Entity::None;
        game.store.delete_entity(entity_id);
    }
    player.energy = 0;
}

/**
 * Construct HaliteImpl from game interface.
 * @param game The game interface.
 */
HaliteImpl::HaliteImpl(Halite &game) : game(game) {}

/**
 * Handle a player command error.
 * @param offenders The set of players this turn who have caused errors.
 * @param commands The player command mapping.
 * @param error The error caused by the player.
 */
void HaliteImpl::handle_error(std::unordered_set<Player::id_type> &offenders,
                              ordered_id_map<Player, std::vector<std::unique_ptr<Command>>> &commands,
                              CommandError error) {
    const auto message = error->log_message();
    const auto &faulty = error->command();
    const auto player_id = error->player;

    // Log the error information.
    if (error->ignored) {
        Logging::log("Player " + to_string(player_id) + " caused warning: " + message, Logging::Level::Warning);
        game.logs.log(player_id, message, PlayerLog::Level::Warning);
    } else {
        Logging::log("Player " + to_string(player_id) + " caused error: " + message, Logging::Level::Error);
        offenders.emplace(player_id);
        game.logs.log(player_id, message, PlayerLog::Level::Error);
    }

    // Find the position of a command within a player's command list.
    auto &player_commands = commands[player_id];
    const auto find_position = [&player_commands](const Command &faulty) {
        return std::find_if(player_commands.begin(), player_commands.end(), [&faulty](const auto &command) {
            return std::addressof(*command) == std::addressof(faulty);
        });
    };

    // Given a command position, log the context of that command.
    const auto log_context = [&player_commands, &game = game, &player_id](const auto position) {
        static constexpr long COMMAND_CONTEXT_LINES = 2L;
        const auto distance = static_cast<long>(std::distance(player_commands.begin(), position));
        const auto commands_begin = player_commands.begin() + std::max(0L, distance - COMMAND_CONTEXT_LINES);
        const auto commands_end = player_commands.begin() +
                std::min(static_cast<long>(player_commands.size()), distance + COMMAND_CONTEXT_LINES + 1);
        for (auto iterator = commands_begin; iterator != commands_end; iterator++) {
            auto number = std::distance(player_commands.begin(), iterator);
            auto marker = iterator == position ? ">>> " : "    ";
            game.logs.log(player_id, marker + std::to_string(number + 1) + "   " + (*iterator)->to_bot_serial());
        }
        game.logs.log(player_id, "");
    };

    // Log the faulty command.
    auto position = find_position(faulty);
    auto distance = std::distance(player_commands.begin(), position);
    game.logs.log(player_id, "At command " + std::to_string(distance + 1) + " of " +
                              std::to_string(player_commands.size()) + ":");
    log_context(position);

    // If there is a context, log the context.
    const auto &context = error->context();
    if (!context.empty()) {
        const auto context_message = error->context_message();
        game.logs.log(player_id, context_message);
        static constexpr size_t MAX_CONTEXT_COMMANDS = 5;
        const auto context_end = context.begin() + std::min(MAX_CONTEXT_COMMANDS, context.size());
        for (auto iterator = context.begin(); iterator != context_end; iterator++) {
            log_context(find_position(*iterator));
        }
        if (context.size() > MAX_CONTEXT_COMMANDS) {
            game.logs.log(player_id, "(suppressing " + std::to_string(context.size() - MAX_CONTEXT_COMMANDS) +
                                     " other commands)\n");
        }
    }
}

}
