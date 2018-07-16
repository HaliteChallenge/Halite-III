#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <unordered_map>
#include <utility>

#include "Entity.hpp"
#include "Enumerated.hpp"
#include "Location.hpp"

namespace hlt {

/** Representation of a Halite player. */
struct Player : public virtual Enumerated<Player> {
    friend class Factory<Player>;

    /** Type of the Entity map of a player, where keys are entity IDs. */
    using Entities = std::unordered_map<Entity::id_type, std::pair<Location, std::reference_wrapper<Entity>>>;

    std::string name;     /**< The name of the player. */
    energy_type energy{}; /**< The amount of energy stockpiled by the player. */
    Location factory;     /**< The factory location of the player. */
    Entities entities{};  /**< Mapping from location of entity to entity */
    std::string command;  /**< The bot command for the player. */

    /**
     * Get whether the player has an entity.
     * @param id The entity ID.
     * @return True if the player has the entity, false otherwise.
     */
    bool has_entity(const Entity::id_type &id) const;

    /**
     * Add an entity.
     * @param entity The entity to add.
     * @param location The location of the entity.
     */
    void add_entity(Entity &entity, Location location);

    /**
     * Remove an entity by ID.
     * @param id The entity ID.
     */
    void remove_entity(const Entity::id_type &id);

    /**
     * Get the location of an entity.
     * @param id The entity ID.
     * @return The entity location.
     */
    Location get_entity_location(const Entity::id_type &id) const;

    /**
     * Get an entity by ID.
     * @param id The entity ID.
     * @return The entity.
     */
    Entity &get_entity(const Entity::id_type &id);

    /**
     * Convert a Player to JSON format.
     * @param[out] json The output JSON.
     * @param player The Player to convert.
     */
    friend void to_json(nlohmann::json &json, const Player &player);

    /**
     * Write a Player to bot serial format.
     * @param ostream The output stream.
     * @param player The Player to write.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const Player &player);

private:
    /**
     * Construct Player from ID, factory, and command.
     * @param id The player ID.
     * @param command The player bot command.
     */
    Player(Player::id_type id, Location factory, std::string command) :
            Enumerated(id), factory(factory), command(std::move(command)) {}

};

}

namespace std {
template<>
struct hash<hlt::Player> : hash<Enumerated<hlt::Player>> {};
}

#endif /* PLAYER_H */
