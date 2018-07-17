#ifndef STORE_HPP
#define STORE_HPP

#include <unordered_set>

#include "Player.hpp"

namespace net {
class Networking;
}

namespace hlt {

/** Storage and lifetime management for Player and Entity objects. */
class Store {
    friend class Halite;
    friend class HaliteImpl;
    friend class net::Networking;

    id_map<Player, Player> players;  /**< Map from player ID to player. */
    id_map<Entity, Entity> entities; /**< Map from entity ID to entity. */

    Factory<Player> player_factory;   /**< The player factory. */
    Factory<Entity> entity_factory;   /**< The entity factory. */
    Factory<Dropoff> dropoff_factory; /**< The dropoff factory. */

    std::unordered_set<Location> changed_cells{}; /**< The cells changed on the last turn. */

public:
    /**
     * Get a player by ID.
     *
     * @param id The player ID.
     * @return The player.
     */
    Player &get_player(const Player::id_type &id);

    /**
     * Get an entity by ID.
     *
     * @param id The entity ID.
     * @return The entity.
     */
    Entity &get_entity(const Entity::id_type &id);

    /**
     * Obtain a new entity.
     *
     * @param energy The energy of the entity.
     * @param owner The owner of the entity.
     * @return The new entity.
     */
    Entity &new_entity(energy_type energy, const Player::id_type &owner);

    /**
     * Obtain a new dropoff.
     * @param location The location of the dropoff.
     * @return The new dropoff.
     */
    Dropoff new_dropoff(Location location);

    /**
     * Delete an entity by ID.
     *
     * @param id The ID of the entity.
     */
    void delete_entity(Entity::id_type id);
};

}

#endif // STORE_HPP
