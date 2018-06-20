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
                entities.erase(entity);
                game->game_map.at(entity->first)->entities.erase(player_pair.first);
                entity++;
            } else {
                entity++;
            }
        }
    }
}

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

void HaliteImpl::initialize_owner_search_from_sprites(std::vector<std::vector<GridOwner>> &ownership_grid, std::queue<Location> &search_cells) {
    for (const auto &player_pair : game->players) {
        for (std::pair<const Location, std::shared_ptr<Entity>> entity_pair : player_pair.second.entities) {
            Location entity_location = entity_pair.first;
            std::shared_ptr<Entity> entity = entity_pair.second;
            const auto CURR_DIST = 0;
            if (!multiple_entities_on_cell(entity_location)) {
                // claim ownership, add to queue to determine neighbors
                ownership_grid[entity_location.second][entity_location.first].owner = entity->owner_id;
                search_cells.push(entity_location);
            } else {
                ownership_grid[entity_location.second][entity_location.first].owner = TIED;
            }
            ownership_grid[entity_location.second][entity_location.first].distance = CURR_DIST;
            // Entity leading to ownership useful in case of ties
            ownership_grid[entity_location.second][entity_location.first].entities.push_back(entity);
        }
    }
}

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
}

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

bool HaliteImpl::multiple_entities_on_cell(Location location) {
    const size_t SINGLE_ENTITY = 1;
    return game->game_map.at(location)->entities.size() > SINGLE_ENTITY;
}

std::vector<Location> HaliteImpl::get_neighbors(Location location) {
    std::vector<Location> neighbors;
    // Allow wrap around neighbors
    neighbors.emplace_back((location.first + 1) % game->game_map.width, location.second);
    neighbors.emplace_back((location.first - 1 + game->game_map.width) % game->game_map.width, location.second);
    neighbors.emplace_back(location.first, (location.second + 1) % game->game_map.height);
    neighbors.emplace_back(location.first, (location.second - 1  + game->game_map.height) % game->game_map.height);
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
}
