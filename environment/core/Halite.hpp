#ifndef HALITE_H
#define HALITE_H

#include "BasicGenerator.hpp"
#include "Command.hpp"
#include "Config.hpp"
#include "Generator.hpp"
#include "Map.hpp"
#include "Networking.hpp"
#include "Player.hpp"

namespace hlt {

class Halite final {
private:
    Config config;                      /**< The game configuration. */
    mapgen::MapParameters parameters;   /**< The map parameters. */
    net::Networking networking;         /**< The networking suite. */

    /** Communicate with bots to obtain moves for next step. */
    void retrieve_moves();

    /** Process all commands for this turn. */
    void process_commands();

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
    // void process_production();

    /**
     * Process production granting for a cell with tied ownership (ie multiple equidistant sprites of different players).
     * @param cell_location: Location object with details of cell with equidistant sprites
     * @param player_entity_list: list of relevant parties in the tie. A relevant party consists of a player and the specific
     * entity they own that is causing the tie
     */
    void process_tie(hlt::Location cell_location, std::list<std::pair<Player*, Entity*>> &player_entity_list);

    /**
     * Process same cell collision of entities
     * @param cell_location: Location object where collusion is taking place
     * @param player_entity_list: list of relevant parties in the collision. A relevant party consists of a player and the
     * entity they own that is on the square
     */
    void process_collision(hlt::Location cell_location, std::list<std::pair<Player*, Entity*>> &player_entity_list);
    /**
     * Merge entities of same player on the same square
     * @param player_entity_list: list of players (should be single player) entity pairs on the same square
     *
     * Side effects: player's list of entities updated with single merged entitiy
     */
    void process_merge(std::list<std::pair<Player*, Entity*>> &player_entity_list);

public:
    unsigned long turn_number{};        /**< The turn number. */
    std::list<Player> players;          /**< The list of players. */
    std::list<Player> living_players;   /**< The list of currently living players. */
    Map game_map;                       /**< The game map. */

    /** Process all production results for this turn. */
    void process_production();

    /**
     * Constructor for the main game.
     *
     * @param config The configuration options for the game.
     * @param parameters The map generation parameters.
     * @param networking_config The networking configuration.
     * @param players The list of players.
     */
    Halite(const Config &config,
           const mapgen::MapParameters &parameters,
           const net::NetworkingConfig &networking_config,
           std::list<Player> players);

    /** Run the game. */
    void run_game();
};

}

#endif
