#include <unordered_set>

#include "Constants.hpp"
#include "BlurTileGenerator.hpp"
#include "Halite.hpp"

namespace hlt {

/** Communicate with bots to obtain moves for next step. */
void Halite::retrieve_moves() {
    // TODO: implement
}

/** Process all commands for this turn. */
void Halite::process_commands() {
    // TODO: implement
}


void Halite::process_tie(const hlt::Location cell_location, std::list<std::pair<Player*, Entity*>> &player_entity_list){
       //TODO: implement
    (void) cell_location;
    (void) player_entity_list;
}

void Halite::process_collision(const hlt::Location cell_location, std::list<std::pair<Player*, Entity*>> &player_entity_list){
        // TODO: implement
    (void) cell_location;
    (void) player_entity_list;
}

void Halite::process_merge(std::list<std::pair<Player*, Entity*>> &player_entity_list){
    // TODO: implement
    (void) player_entity_list;
}

/** General Production calculation algorithm:
 * Go through every cell on map. For each cell, go through all entities. Keep track of closest distance seen, and
 * closest player + entity (need both as entity does not know player it belongs to). If we get multiple entities
 * the same distance from the cell, track both entities
 *
 * After checking a cell, process merges of same-player sprites sharing a cell, collisions of different player sprites
 * on the same cell, and break ties.
 *
 * Player's production totals are updated dynamically. Player entity lists may also be changed in this function
 */
void Halite::process_production() {

    for (auto pos_y = 0; pos_y < game_map.height; ++pos_y) {
        for (auto pos_x = 0; pos_x < game_map.width; ++pos_x) {
            // Start close dist at value higher than any possible_dist
            auto closest_dist = game_map.height + game_map.width;

            // Close player-entity pairs are collected to forward to other functions to deal with collisions and ties
            // As we want these functions to be able to modify the production of players, we need to collect ptrs, not copies of objects
            // Entities are also collected by reference in the event that collisions will impact their energy
            std::list< std::pair<Player*, Entity*> > close_players;

            // Algorithms for collision and equidistant players may dependent on the qualities of the entities involved
            // Thus, we want to keep track of both a player and the entity at a close distance
            // However, we don't really care if the same player has two sprites equidistant from a cell, so keep track
            // of how many unique players closest to a cell there are in addition to the actual entity
            std::unordered_set<long> unique_player_ids;
            const hlt::Location cell_location = Location(pos_x, pos_y);

            // TODO: Determine whether its more appropriate to iterate through players or living players - KB
            for (Player &player : players) {
                for (Entity &entity : player.entities) {
                    const auto cell_player_dist = cell_location.distance(entity.location, game_map.width, game_map.height);
                    if (cell_player_dist < closest_dist) {
                        // remove any farther players from list, then add this one
                        close_players.clear();
                        closest_dist = cell_player_dist;
                        close_players.emplace_back(&player, &entity);

                        unique_player_ids.clear();
                        unique_player_ids.insert(player.player_id);
                    } else if (cell_player_dist == closest_dist) {
                        // Players' sprites are the same distance away, so just add the pair to the list
                        close_players.emplace_back(&player, &entity);
                        unique_player_ids.insert(player.player_id);
                    }
                }
            }
            // Process production granting, merging, and tie breaking for this cell
            if (close_players.size() > 1) {
                if (hlt::GameConstants::get().ALLOW_ENTITY_MERGE) {
                    process_merge(close_players);
                }
            }
            // Only have 1 closest player, so that player gets the production of the cell
            if (unique_player_ids.size() == 1) {
                Player *closest_player = close_players.front().first;
                closest_player->energy += game_map.grid[pos_y][pos_x]->production();
            }

            if (closest_dist == 0) {
                // If entities can merge, do so
                // TODO: Determine correct ordering of merging and declaring tiebreakers - KB
                // Depending on tie breaker used, merge may impact who wins a tie, but merged entities are currently NOT
                // reflected in information passed to tiebreaker function
                if (close_players.size() > 1 && hlt::GameConstants::get().ALLOW_ENTITY_MERGE) process_merge(close_players);

                // we have a collision! Add to list of collisions and pass off to different function for modularity
                if (unique_player_ids.size() > 1) process_collision(cell_location, close_players);

            } else if (close_players.size() > 1 && closest_dist > 0) {
                // Cell is tied for distance. Add to list of cell with dist ties and pass off to different function for modularity
                process_tie(cell_location, close_players);
            } else {
                // TODO: error case, no sprites on board - KB
            }
        }
    }
}

/** Run the game. */
void Halite::run_game() {
    for (auto &player : players) {
        networking.initialize_player(player);
    }
    const auto &constants = GameConstants::get();
    for (turn_number = 0; turn_number < constants.MAX_TURNS; turn_number++) {
        for (auto &player : players) {
            Command command;
            networking.handle_frame(player, command);
        }
    }

    // TODO: start main event loop
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
        config(config), parameters(parameters),
        networking(net::Networking(networking_config, this)), players(players),
        game_map(mapgen::BlurTileGenerator(parameters).generate(players)) {}

}
