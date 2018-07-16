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
    template<class K, class V>
    using id_map = std::unordered_map<typename K::id_type, V>;

    /** Type of storage maps from IDs to objects. */
    template<class T>
    using Store = id_map<T, T>;

    /** Mappings from ID to objects, also serving as owners for Player and Entity. */
    Store<Player> players;                                 /**< Map from player ID to player. */
    Store<Entity> entities;                                /**< Map from entity ID to entity. */
    id_map<Entity, std::reference_wrapper<Player>> owners; /**< Map from entity ID to entity owner. */

    /** Game object factories. */
    Factory<Player> player_factory;   /**< The player factory. */
    Factory<Entity> entity_factory;   /**< The entity factory. */

    /** Transient game state. */
    unsigned long turn_number{};      /**< The turn number. */

    /** External game state. */
    Map &map;                         /**< The game map. */
    GameStatistics &game_statistics;  /**< The statistics of the game. */
    Replay &replay;                   /**< Replay instance to collect info for visualizer. */

    /** Configuration. */
    Config config;                        /**< The game configuration. */

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
     * @param player_commands The list of player commands.
     * @param game_statistics The game statistics to use.
     * @param replay The game replay to use.
     */
    Halite(const Config &config,
           Map &map,
           const net::NetworkingConfig &networking_config,
           const std::vector<std::string> &player_commands,
           GameStatistics &game_statistics,
           Replay &replay);

    /** Run the game. */
    void run_game();

    /**
     * Load a snapshot.
     * @param snapshot The snapshot.
     */
    void load_snapshot(const Snapshot &snapshot);

    /**
     * Get a player by ID.
     *
     * @param id The player ID.
     * @return The player.
     */
    Player &get_player(const Player::id_type &id);

    /**
     * Get the owner of an entity.
     *
     * @param id The entity ID.
     * @return The owner of the entity.
     */
    Player &get_owner(const Entity::id_type &id);

    /**
     * Obtain a new entity.
     *
     * @param energy The energy of the entity.
     * @param player The owner of the player.
     * @param location The location of the entity.
     * @return The new entity.
     */
    Entity &new_entity(energy_type energy, Player &player, Location location);

    /**
     * Delete an entity by ID.
     *
     * @param id The ID of the entity.
     */
    void delete_entity(const Entity::id_type &id);

    /**
     * Get an entity by ID.
     *
     * @param id The entity ID.
     * @return The entity.
     */
    Entity &get_entity(const Entity::id_type &id);

    /** Default destructor is defined where HaliteImpl is complete. */
    ~Halite();
};

}

#endif
