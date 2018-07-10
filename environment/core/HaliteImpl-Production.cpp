#include "HaliteImpl.hpp"

namespace hlt {

/**
 * Update the energy/production of each player by calculating ownership of each cell in the current turn.
 * General production calculation algorithm:
 * Use modified BFS starting from each sprite to determine cell ownership one distance unit at a time.
 */
void HaliteImpl::process_production() {
    this->ownership_grid = OwnershipGrid(game.game_map.width, game.game_map.height);
    std::queue<Location> search_cells;
    initialize_owner_search_from_sprites(search_cells);
    run_initialized_owner_search(search_cells);
    update_production_from_ownership();
}

/**
 * Initialized ownership grid with locations of sprites. Assigns each cell to be owned by the sprite on it or determines
 * that there is a tie if two sprites share a cell. Initializes search_cells queue with cells with sprites on them.
 *
 * @param search_cells Queue of cells that have been assigned an owner. After a cell's owner has been determined, it is added
 * to search_cells for the purpose of searching its neighbors.
 */
void HaliteImpl::initialize_owner_search_from_sprites(std::queue<Location> &search_cells) {
    static constexpr auto INITIAL_DISTANCE = 0;
    for (const auto &[_, player] : game.players) {
        if (!player.alive) continue;

        for (const auto &[location, entity] : player.entities) {
            auto &cell = ownership_grid[location];
            if (multiple_entities_on_cell(location)) {
                cell.owner = process_collision(location);
            } else {
                cell.owner = entity->owner_id;
            }
            if (cell.owner != CellOwner::UNOWNED) {
                // Add to queue to determine neighbors
                search_cells.push(location);
            }

            cell.distance = INITIAL_DISTANCE;
            // Entity leading to ownership useful in case of ties
            cell.entities.insert(entity);
        }
    }
}

/**
 * Run modified BFS algorithm to determine owner (or tie) of each cell. Assumes that search cells has been initialized
 * with locations with sprites on them.
 *
 * @param search_cells Queue of cells that have been assigned an owner. After a cell's owner has been determined, it is added
 * to search cells to search the neighbors of said cells.
 */
void HaliteImpl::run_initialized_owner_search(std::queue<Location> &search_cells) {
    while (!search_cells.empty()) {
        Location current_location = search_cells.front();
        const auto &neighbors = game.game_map.get_neighbors(current_location);
        // Search through unowned neighbor of owned cell and determine their ownership
        for (const auto &neighbor : neighbors) {
            if (ownership_grid[neighbor].owner == CellOwner::UNOWNED) {
                determine_cell_ownership(neighbor);
                search_cells.push(neighbor);
            }
        }
        search_cells.pop();
    }
}

/**
 * Grant production from owned cells on this turn to players.
 * Calculates total production gained on this turn for each player before adding to players' totals.
 */
void HaliteImpl::update_production_from_ownership() {
    // Calculate total production per player for this turn for statistics and scoring
    std::unordered_map<Player::id_type, energy_type> turn_player_production;
    for (dimension_type y_position = 0; y_position < game.game_map.height; ++y_position) {
        for (dimension_type x_position = 0; x_position < game.game_map.width; ++x_position) {
            switch (ownership_grid.at(x_position, y_position).owner) {
            case CellOwner::UNOWNED:
                // TODO: error case
                break;
            case CellOwner::TIED:
                process_tie({x_position, y_position}, ownership_grid.at(x_position, y_position).entities,
                            turn_player_production);
                break;
            default:
                const auto &current_cell = ownership_grid.at(x_position, y_position);
                turn_player_production[current_cell.owner] += game.game_map.at(x_position, y_position)->production();
            }
        }
    }
    // Add the energy from this turn to each player
    for (const auto &[player_id, energy] : turn_player_production) {
        if (!this->game.players[player_id].alive) continue;
        game.players[player_id].energy += energy;
    }
    update_player_stats(turn_player_production);
}

/**
 * Determines the proper owner of a cell (or that the cell has tied ownership).
 * Assumes cell_location neighbors at least one owned cell.
 *
 * @param cell_location Location of cell to determine ownership.
 */
void HaliteImpl::determine_cell_ownership(const Location &cell_location) {
    dimension_type closest_owned_distance = std::numeric_limits<dimension_type>::max();
    bool multiple_close_players = false;
    std::unordered_set<std::shared_ptr<PlayerEntity>> close_entities;
    Player::id_type closest_cell_owner = CellOwner::UNOWNED;
    const auto &neighbors = game.game_map.get_neighbors(cell_location);
    for (const auto &neighbor : neighbors) {
        // Find the owned neighbor with closest distance. If multiple neighbors same distance,
        const auto &cell = ownership_grid[neighbor];
        if (cell.owner != CellOwner::UNOWNED) {
            if (cell.distance < closest_owned_distance) {
                closest_owned_distance = cell.distance;
                multiple_close_players = false;
                close_entities = cell.entities;
                closest_cell_owner = cell.owner;
            } else if (cell.distance == closest_owned_distance) {
                // Determine if equidistant sprites are owned by same player, in which case don't consider cell tied
                // (though do track all entities this distance away)
                multiple_close_players = multiple_close_players || closest_cell_owner != cell.owner;
                close_entities.insert(cell.entities.begin(), cell.entities.end());
            }
        }
    }
    // This cell is one unit farther than the cell it's closest to.
    auto &cell = ownership_grid[cell_location];
    cell.distance = ++closest_owned_distance;
    cell.entities = close_entities;
    cell.owner = multiple_close_players ? CellOwner::TIED : closest_cell_owner;
}

/**
 * Determine whether multiple entities are on a cell.
 * @param location The location of the cell.
 * @return True if greater than 1 entity is on the cell.
 */
bool HaliteImpl::multiple_entities_on_cell(const Location &location) const {
    static constexpr auto SINGLE_ENTITY = 1;
    return game.game_map.at(location)->entities.size() > SINGLE_ENTITY;
}

/**
 * Process production granting for a cell with tied ownership
 * (i.e. multiple equidistant sprites of different players).
 * @param cell_location Location object with details of cell with equidistant sprites.
 * @param close_entities Entities calculated to be at the same closest distance to relevant cell.
 * @param turn_player_production Mapping from player ID to production energy they gain during the turn.
 *     Will be updated to grant production of current cell to player that wins the tie.
 */
void HaliteImpl::process_tie(const Location &cell_location, std::unordered_set<std::shared_ptr<PlayerEntity>> &close_entities,
                             std::unordered_map<Player::id_type, energy_type> &turn_player_production) {
    // TODO: implement
    (void) cell_location;
    (void) close_entities;
    (void) turn_player_production;
}

/**
 * Process the search initialization for a cell with colliding entities
 * (i.e. entities from multiple different players on one cell).
 *
 * @param cell_location Location of cell with colliding entities.
 * @return Resulting owner for this cell.
 */
Player::id_type HaliteImpl::process_collision(const Location &cell_location) {
    // Use the dominant-entity method
    auto tied = false;
    auto max_energy = std::numeric_limits<energy_type>::min();
    // Pick an arbitrary player to start
    Player::id_type max_player = game.game_map[cell_location]->entities.begin()->first;
    // Find the entity with highest energy, keeping track of ties
    for (auto &[player_id, entity] : game.game_map[cell_location]->entities) {
        if (entity->energy > max_energy) {
            max_energy = entity->energy;
            max_player = player_id;
            tied = false;
        } else if (entity->energy == max_energy) {
            tied = true;
        }
    }
    if (tied) {
        return CellOwner::TIED;
    } else {
        return max_player;
    }
}

}
