#ifndef HALITE_H
#define HALITE_H

#include "Config.hpp"
#include "Networking.hpp"
#include "Store.hpp"
#include "mapgen/Generator.hpp"

namespace hlt {

struct GameStatistics;

class HaliteImpl;

struct Replay;

struct Snapshot;

/** Halite game interface, exposing the top level of the game. */
class Halite final {
    /** Transient game state. */
    unsigned long turn_number{};      /**< The turn number. */
    Store store;                      /**< The entity store. */

    /** External game state. */
    Map &map;                         /**< The game map. */
    GameStatistics &game_statistics;  /**< The statistics of the game. */
    Replay &replay;                   /**< Replay instance to collect info for visualizer. */

    /** Configuration. */
    Config config;                    /**< The game configuration. */

    /** Friend classes have full access to game state. */
    friend class net::Networking;

    friend class HaliteImpl;

    net::Networking networking;       /**< The networking suite. */
    std::unique_ptr<HaliteImpl> impl; /**< The pointer to implementation. */

public:
    id_map<Player, std::ostringstream> error_logs; /**< Player error logs. */

    /**
     * Constructor for the main game.
     *
     * @param config The configuration options for the game.
     * @param map The game map.
     * @param networking_config The networking configuration.
     * @param game_statistics The game statistics to use.
     * @param replay The game replay to use.
     */
    Halite(const Config &config,
           Map &map,
           const net::NetworkingConfig &networking_config,
           GameStatistics &game_statistics,
           Replay &replay);

    /**
     * Run the game.
     * @param player_commands The list of player commands.
     * @param snapshot A snapshot of game state.
     */
    void run_game(const std::vector<std::string> &player_commands,
                  const Snapshot &snapshot);

    /** Remove a player from the game. */
    void kill_player(const Player::id_type& player_id);

    /** Get a player from the game. Used for writing error logs */
    const Player& get_player(Player::id_type player_id);

    /** Generate a snapshot string from current game state. */
    std::string to_snapshot(const mapgen::MapParameters& map_parameters);

    /**
     * Add a section to a player's error log.
     */
    void log_error_section(Player::id_type id, const std::string& section_name);

    /**
     * Add a line to this player's error log.
     */
    void log_error(Player::id_type id, const std::string& text);

    /** Default destructor is defined where HaliteImpl is complete. */
    ~Halite();
};

}

#endif
