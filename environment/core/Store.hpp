#ifndef STORE_HPP
#define STORE_HPP

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

    id_map<Player, Player> players;         /**< Map from player ID to player. */
    id_map<Entity, Entity> entities;        /**< Map from entity ID to entity. */
    id_map<Entity, Player::id_type> owners; /**< Map from entity ID to owner. */

    Factory<Player> player_factory; /**< The player factory. */
    Factory<Entity> entity_factory; /**< The entity factory. */

public:
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
    Player::id_type get_owner(const Entity::id_type &id);

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
     * Delete an entity by ID.
     *
     * @param id The ID of the entity.
     */
    void delete_entity(Entity::id_type id);
};

}

#endif // STORE_HPP
