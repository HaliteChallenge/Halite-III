#include "HaliteImpl.hpp"

namespace hlt {

/** Communicate with bots to obtain moves for next step. */
void HaliteImpl::retrieve_moves() {
    for (auto &player : game->players) {
        Command command;
        game->networking.handle_frame(player, command);
    }
}

/** Process all commands for this turn. */
void HaliteImpl::process_commands() {
    // TODO: implement
}

/** Process all entity lifecycle events for this turn. */
void HaliteImpl::process_entities() {
    // TODO: implement
}


void HaliteImpl::process_tie(Location cell_location, std::vector<std::shared_ptr<Entity>> &close_entities,
                             std::unordered_map<id_type, energy_type> &turn_player_production){
    // TODO: implement
    (void) cell_location;
    (void) close_entities;
    (void) turn_player_production;
}

void HaliteImpl::find_closest_entities(Location cell_location, std::vector<std::shared_ptr<Entity>> &close_entities) {
    auto closest_dist = game->game_map.height + game->game_map.width;
    for (const Player &player : game->players) {
        for (auto &loc_entity_pair : player.entities) {
            std::pair<dimension_type, dimension_type> entity_loc = loc_entity_pair.first;
            std::shared_ptr<Entity> curr_entity = loc_entity_pair.second;
            dimension_type cell_player_dist = game->game_map.distance(entity_loc.first, entity_loc.second, cell_location.pos_x, cell_location.pos_y);
            if (cell_player_dist < closest_dist) {
                // remove any farther players from list, then add this one
                close_entities.clear();
                closest_dist = cell_player_dist;
                close_entities.push_back(curr_entity);
            } else if (cell_player_dist == closest_dist) {
                // Players' sprites are the same distance away, so just add the close entity to the list
                close_entities.push_back(curr_entity);
            }
        }
    }
}

/**
 * General Production calculation algorithm:
 * Go through every cell on map. For each cell, go through all entities. Keep track of closest distance seen, and
 * which entities were that close. If we get multiple entities the same distance from the cell, track both entities.
 *
 * After checking a cell, determine if there is a closest distance tie between multiple players, and if so, send information
 * to a different function to break ties (for modularity of tie breaking decision).
 *
 * Player's production totals are updated dynamically at the end of the function, but total production gain in this turn
 * for each player is calculated for  use in scoring and statistics.
 */
void HaliteImpl::process_production() {
    std::unordered_map<id_type, energy_type> turn_player_production;
    for (auto pos_y = 0; pos_y < game->game_map.height; ++pos_y) {
        for (auto pos_x = 0; pos_x < game->game_map.width; ++pos_x) {
            auto cell_location = Location{pos_x, pos_y};
            std::vector<std::shared_ptr<Entity>> close_entities;
            find_closest_entities(cell_location, close_entities);
            if (!close_entities.empty()) {
                // determine how many different players are this distance away
                id_type first_player_id = close_entities.front()->owner_id;
                bool multiple_close_players = false;
                for (const std::shared_ptr<Entity> &entity : close_entities) {
                    multiple_close_players = entity->owner_id != first_player_id;
                }
                if (multiple_close_players) {
                    // Process ties (ie multiple equidistant players) in new function to increase modularity of tie behavior
                    process_tie(Location(pos_x, pos_y), close_entities, turn_player_production);
                } else {
                    turn_player_production[first_player_id] += game->game_map.grid[pos_y][pos_x]->production();
                }
            }
        }
    }

    // Add the energy from this turn to each player
    for (Player &player : game->players) {
        player.energy += turn_player_production[player.player_id];
    }
}

}
