#include <algorithm>

#include "HaliteImpl.hpp"
#include <iomanip>
#include <limits>

namespace hlt {

/**
 * Communicate with bots to obtain commands for next step.
 * @return Commands received from players.
 */
std::unordered_map<id_type, Command> HaliteImpl::retrieve_commands() {
    std::unordered_map<id_type, Command> commands;
    for (auto &player : game->players) {
        Command command;
        game->networking.handle_frame(player.second, command);
        commands[player.first] = std::move(command);
    }
    return commands;
}

/**
 * Process the effects of commands.
 * @param commands The commands to process.
 */
void HaliteImpl::process_commands(const std::unordered_map<id_type, Command> &commands) {
    for (const auto &player_command : commands) {
        auto &player = game->players[player_command.first];
        auto &command = player_command.second;
        command->act_on_map(game->game_map, player);
        game->replay_struct.full_frames.back().moves = commands;
    }
}

/**
 * Process a spawn operation at a location, possibly merging onto an existing entity.
 * @param player The player owning the new entity.
 * @param location The location at which to spawn.
 */
void HaliteImpl::spawn_entity(Player &player, const Location& location) {
    const auto &constants = Constants::get();
    auto &entities = player.entities;
    auto entity_iterator = entities.find(location);

    if (entity_iterator != entities.end()) {
        // If there is already an entity, merge.
        entity_iterator->second->energy += constants.NEW_ENTITY_ENERGY;
    } else {
        // Otherwise, spawn.
        auto entity = make_entity<Entity>(player.player_id, constants.NEW_ENTITY_ENERGY);
        entities[location] = entity;
        game->game_map.at(location)->entities[player.player_id] = std::move(entity);
    }
    // Create new game event for replay file, regardless of whether spawn creates a new entity or adds to an old entity
    GameEvent spawn_event = std::make_unique<SpawnEvent>(location, constants.NEW_ENTITY_ENERGY, player.player_id);
    game->replay_struct.full_frames.back().events.push_back(std::move(spawn_event));
}

/** Process all entity lifecycle events for this turn. */
void HaliteImpl::process_entities() {
    // Each factory may spawn a new entity
    const auto &constants = Constants::get();
    for (auto &player_pair : game->players) {
        auto &player = player_pair.second;
        if (player.energy >= constants.NEW_ENTITY_ENERGY_COST) {
            player.energy -= constants.NEW_ENTITY_ENERGY_COST;
            spawn_entity(player, player.factory_location);
        }
    }
    // Each entity loses some health, each entity with no remaining energy is removed
    for (auto &player_pair : game->players) {
        auto &entities = player_pair.second.entities;
        for (auto entity = entities.begin(); entity != entities.end();) {
            entity->second->energy -= constants.BASE_TURN_ENERGY_LOSS;
            if (entity->second->energy <= 0) {
                game->game_map.at(entity->first)->remove_entity(player_pair.second);
                // Create a new death event for replay file
                GameEvent death_event = std::make_unique<DeathEvent>(entity->first, entity->second->owner_id);
                game->replay_struct.full_frames.back().events.push_back(std::move(death_event));
                entities.erase(entity++);
            } else {
                entity++;
            }
        }
    }
}

/**
 * Process production granting for a cell with tied ownership (ie multiple equidistant sprites of different players).
 * @param cell_location: Location object with details of cell with equidistant sprites
 * @param close entities Entities calculated to be at the same closest distance to relevant cell
 * @param turn_player_production Mapping from player id to production energy they gain during the turn.
 *     Will be updated to grant production of current cell to player that wins the tie
 */
void HaliteImpl::process_tie(Location cell_location, std::vector<std::shared_ptr<Entity>> &close_entities,
                             std::unordered_map<id_type, energy_type> &turn_player_production){
    // TODO: implement
    (void) cell_location;
    (void) close_entities;
    (void) turn_player_production;
}

/**
 * General Production calculation algorithm:
 * Use modified BFS starting from each sprite to determine cell ownership one distance unit at a time.
 */
void HaliteImpl::process_production() {
    std::vector<std::vector<GridOwner>>
            ownership_grid(static_cast<unsigned long> (game->game_map.height),
                           std::vector<GridOwner>(static_cast<unsigned long> (game->game_map.width), GridOwner{}));
    std::queue<Location> search_cells;
    initialize_owner_search_from_sprites(ownership_grid, search_cells);
    run_initialized_owner_search(ownership_grid, search_cells);
    update_production_from_ownership(ownership_grid);
}

/**
 * Initialized ownership grid with locations of sprites. Assigns each cell to be owned by the sprite on it or determines
 * that there is a tie if two sprites share a cell. Initializes search_cells queue with cells with sprites on them.
 *
 * @param ownership_grid 2D grid in shape of game map storing details of cell ownership in a specific turn. Will update
 * ownership details as the search determines them.
 * @param search_cells Queue of cells that have been assigned an owner. After a cell's owner has been determined, it is added
 * to search_cells for the purpose of searching its neighbors.
 */
void HaliteImpl::initialize_owner_search_from_sprites(std::vector<std::vector<GridOwner>> &ownership_grid, std::queue<Location> &search_cells) {
    for (const auto &player_pair : game->players) {
        for (std::pair<const Location, std::shared_ptr<Entity>> entity_pair : player_pair.second.entities) {
            Location entity_location = entity_pair.first;
            std::shared_ptr<Entity> entity = entity_pair.second;
            const auto CURRENT_DISTANCE = 0;
            if (!multiple_entities_on_cell(entity_location)) {
                // claim ownership, add to queue to determine neighbors
                ownership_grid[entity_location.second][entity_location.first].owner = entity->owner_id;
                search_cells.push(entity_location);
            } else {
                ownership_grid[entity_location.second][entity_location.first].owner = TIED;
            }
            ownership_grid[entity_location.second][entity_location.first].distance = CURRENT_DISTANCE;
            // Entity leading to ownership useful in case of ties
            ownership_grid[entity_location.second][entity_location.first].entities.push_back(entity);
        }
    }
}

/**
 * Run modified BFS algorithm to determine owner (or tie) fof each cell. Assumes that search cells has been initized
 * with locations with sprites on them.
 *
 * @param ownership_grid 2D grid in shape of game map storing details of cell ownership in a specific turn. Will update
 * ownership details as the search determines them.
 * @param search_cells Queue of cells that have been assigned an owner. After a cell's owner has been determined, it is added
 * to search cells to search the neighbors of said cells.
 */
void HaliteImpl::run_initialized_owner_search(std::vector<std::vector<GridOwner>> &ownership_grid, std::queue<Location> &search_cells) {
    while (!search_cells.empty()) {
        Location current_location = search_cells.front();
        std::vector<Location> neighbors = get_neighbors(current_location);
        GridOwner current_cell = ownership_grid[current_location.second][current_location.first];
        // Search through unowned neighbor of owned cell and determine their ownership
        for (Location &neighbor : neighbors) {
            if (ownership_grid[neighbor.second][neighbor.first].owner == UNOWNED) {
                determine_cell_ownership(neighbor, ownership_grid);
                search_cells.push(neighbor);
            }
        }
        search_cells.pop();
    }
}

/**
 * Grant production from owned cells on this turn to players
 *
 * Calculates total production gained on this turn for each player before adding to players' totals.
 * @param ownership_grid: 2d grid with each cell having an owner.
 */
void HaliteImpl::update_production_from_ownership(std::vector<std::vector<GridOwner>> &ownership_grid) {
    // Calculate total production per player for this turn for statistics and scoring
    std::unordered_map<id_type, energy_type> turn_player_production;
    for (dimension_type y_position = 0; y_position < game->game_map.height; ++y_position) {
        for (dimension_type x_position = 0; x_position <  game->game_map.width; ++x_position) {
            switch(ownership_grid[y_position][x_position].owner) {
                case UNOWNED :
                    // TODO: error case
                    break;
                case TIED :
                    process_tie(std::make_pair(x_position, y_position), ownership_grid[y_position][x_position].entities, turn_player_production);
                    break;
                default :
                    GridOwner current_cell = ownership_grid[y_position][x_position];
                    turn_player_production[current_cell.owner] += game->game_map.at(x_position, y_position)->production();
            }
        }
    }
    // Add the energy from this turn to each player
    for (const auto &player_id_energy_pair : turn_player_production) {
        game->players[player_id_energy_pair.first].energy += player_id_energy_pair.second;
    }
    update_player_stats(turn_player_production);
}

/**
 * Determines the proper owner of a cell (or that the cell has tied ownership. Assumes cell_location neighbors at least
 * one owned cell
 *
 * @param cell_location Location of cell to determine ownership
 * @param ownership_grid 2D Grid storing details of cell ownership.
 */
void HaliteImpl::determine_cell_ownership(Location cell_location, std::vector<std::vector<GridOwner>> &ownership_grid) {
    dimension_type closest_owned_distance = std::numeric_limits<dimension_type>::max();
    bool multiple_close_players = false;
    std::vector<std::shared_ptr<Entity>> close_entities;
    id_type closest_cell_owner = UNOWNED;
    std::vector<Location> neighbors = get_neighbors(cell_location);
    for (const auto neighbor : neighbors) {
        // Find the owned neighbor with closest distance. If multiple neighbors same distance,
        if (ownership_grid[neighbor.second][neighbor.first].owner != UNOWNED) {
            GridOwner cell = ownership_grid[neighbor.second][neighbor.first];
            if (cell.distance < closest_owned_distance) {
                closest_owned_distance = cell.distance;
                multiple_close_players = false;
                close_entities = cell.entities;
                closest_cell_owner = cell.owner;
            } else if (cell.distance == closest_owned_distance) {
                // Determine if equidistant sprites are owned by same player, in which case don't consider cell tied
                // (though do track all entities this distance away)
                multiple_close_players = multiple_close_players || closest_cell_owner != cell.owner;
                close_entities.insert(close_entities.end(), cell.entities.begin(), cell.entities.end());

            }
        }
    }
    // This cell is one unit farther than the cell its closest to.
    ownership_grid[cell_location.second][cell_location.first].distance = closest_owned_distance + 1;
    ownership_grid[cell_location.second][cell_location.first].entities = close_entities;
    ownership_grid[cell_location.second][cell_location.first].owner = multiple_close_players ? TIED : closest_cell_owner;
}

/**
 * Checks a cell to determine if multiple entities are on the cell
 * @param location. The location of the cell
 * @return Bool indicating if greater than 1 entity is on the cell.
 */
bool HaliteImpl::multiple_entities_on_cell(Location location) {
    const size_t SINGLE_ENTITY = 1;
    return game->game_map.at(location)->entities.size() > SINGLE_ENTITY;
}

/**
 * Given a location of a cell, return its 4 neighbors
 * @param location: the location of the cell we want the neighbors of
 * @return Vector of locations of length 4. A neighbor is a location with manhattan distance 1 from the first location.
 *  This function encapsulates the wrap around map - ie cell 0,0's neighbor's include cells at the very bottom and very right
 */
std::vector<Location> HaliteImpl::get_neighbors(Location location) {
    std::vector<Location> neighbors;
    // Allow wrap around neighbors
    neighbors.emplace_back((location.first + 1) % game->game_map.width, location.second);
    neighbors.emplace_back((location.first - 1 + game->game_map.width) % game->game_map.width, location.second);
    neighbors.emplace_back(location.first, (location.second + 1) % game->game_map.height);
    neighbors.emplace_back(location.first, (location.second - 1 + game->game_map.height) % game->game_map.height);
    return neighbors;
}

/**
 * Determine whether the game has ended.
 *
 * @return True if the game has ended.
 */
bool HaliteImpl::game_ended() const {
    long num_alive_players = 0;
    for (auto &player_entry : game->players) {
        // TODO: implement edge case of last player being unable to produce
        if (!player_entry.second.entities.empty()) {
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
 * @param productions Mapping from player id to the production they gained in the current turn.
 */
void HaliteImpl::update_player_stats(std::unordered_map<id_type, energy_type> productions) {
    for (PlayerStatistics &player_stats : game->game_statistics.player_statistics) {
        // Player with sprites is still alive, so mark as alive on this turn and add production gained
        if (!game->players[player_stats.player_id].entities.empty()) {
            player_stats.last_turn_alive = game->turn_number;
            player_stats.turn_productions.push_back(productions[player_stats.player_id]);
            player_stats.total_production += productions[player_stats.player_id];
        } else {
            player_stats.turn_productions.push_back(0);
        }
    }
}

/**
 * Compare two players to rank them. Returns true if player1 will rank below player 2.
 *
 * @param player1 The statistics of the first player
 * @param player2 The statistics of the second player
 * @return bool that is true if player1 ranks below (ie is worse than) player2
 */
bool compare_players(PlayerStatistics player1, PlayerStatistics player2) {
    if (player1.last_turn_alive == player2.last_turn_alive) {
        auto turn_to_compare = static_cast<long> (player1.last_turn_alive);
        while (player1.turn_productions[turn_to_compare] == player2.turn_productions[turn_to_compare]){
            --turn_to_compare;
            if (turn_to_compare < 0) {
                return true;
            }
        }
        return player1.turn_productions[turn_to_compare] < player2.turn_productions[turn_to_compare];
    } else {
        return player1.last_turn_alive < player2.last_turn_alive;
    }
}

/**
 * Update players' rankings based on their final turn alive, then break ties with production totals in final turn.
 * Function is intended to be called at end of game, and will in place modify the ranking field of player statistics
 * to rank players from winner (1) to last player
 */
void HaliteImpl::rank_players() {
    std::stable_sort(game->game_statistics.player_statistics.begin(), game->game_statistics.player_statistics.end(), compare_players);
    // Reverse list to have best players first
    std::reverse(game->game_statistics.player_statistics.begin(), game->game_statistics.player_statistics.end());

    for (size_t index = 0; index < game->game_statistics.player_statistics.size(); ++index) {
        game->game_statistics.player_statistics[index].rank = index + 1l;
    }
}

}
