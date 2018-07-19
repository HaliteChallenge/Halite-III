#ifndef HALITE_H
#define HALITE_H

#include "Config.hpp"
#include "Networking.hpp"
#include "Store.hpp"

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
     */
    void run_game(const std::vector<std::string> &player_commands);

    /**
     * Load a snapshot.
     * @param snapshot The snapshot.
     */
    void load_snapshot(const Snapshot &snapshot);

    /** Remove a player from the game. */
    void kill_player(const Player::id_type& player_id);

    /** Get a player from the game. Used for writing error logs */
    const Player& get_player(Player::id_type player_id);

    /** Generate a snapshot string from current game state. */
    std::string to_snapshot();

    /** Default destructor is defined where HaliteImpl is complete. */
    ~Halite();
};

}

#endif
