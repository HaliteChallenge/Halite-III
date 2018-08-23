#ifndef HALITE_H
#define HALITE_H

#include "Networking.hpp"
#include "PlayerLog.hpp"
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

    /** Friend classes have full access to game state. */
    friend class net::Networking;

    friend class HaliteImpl;

    net::Networking networking;       /**< The networking suite. */
    std::unique_ptr<HaliteImpl> impl; /**< The pointer to implementation. */
    std::mt19937 rng;                 /** The random number generator used for tie breaking. */

public:
    PlayerLogs logs;                  /**< The player logs. */

    /**
     * Constructor for the main game.
     *
     * @param map The game map.
     * @param networking_config The networking configuration.
     * @param game_statistics The game statistics to use.
     * @param replay The game replay to use.
     */
    Halite(Map &map,
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

    /** Generate a snapshot string from current game state. */
    std::string to_snapshot(const mapgen::MapParameters &map_parameters);

    /** Default destructor is defined where HaliteImpl is complete. */
    ~Halite();
};

}

#endif
