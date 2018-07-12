#ifndef HALITE_H
#define HALITE_H

#include "BasicGenerator.hpp"
#include "Command.hpp"
#include "Config.hpp"
#include "Generator.hpp"
#include "Map.hpp"
#include "Networking.hpp"
#include "Player.hpp"
#include "Location.hpp"
#include "Statistics.hpp"
#include "Replay.hpp"
#include "Snapshot.hpp"

namespace hlt {

class HaliteImpl;

/** Halite game interface, exposing the top level of the game. */
class Halite final {
    friend class HaliteImpl;

public:
    unsigned long turn_number{};                          /**< The turn number. */
    std::unordered_map<Player::id_type, Player> players;  /**< Map from player ID to player. */
    std::unordered_map<Entity::id_type, Entity> entities; /**< Map from entity ID to entity. */
    Map game_map;                                         /**< The game map. */
    GameStatistics game_statistics;                       /**< The statistics of the game. */
    Replay replay;                                        /**< Replay instance to collect info for visualizer. */

private:
    Config config;                    /**< The game configuration. */
    mapgen::MapParameters parameters; /**< The map parameters. */
    net::Networking networking;       /**< The networking suite. */
    std::unique_ptr<HaliteImpl> impl; /**< The pointer to implementation. */

public:
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
           std::vector<Player> players);

    /** Run the game. */
    void run_game();

    /**
     * Load a snapshot.
     * @param snapshot The snapshot.
     */
    void load_snapshot(const Snapshot &snapshot);

    /** Default destructor is defined where HaliteImpl is complete. */
    ~Halite();
};

}

#endif
