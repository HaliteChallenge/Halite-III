#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <unordered_map>
#include <utility>

#include "Dropoff.hpp"
#include "Entity.hpp"
#include "Enumerated.hpp"
#include "Location.hpp"

namespace hlt {

/** Representation of a Halite player. */
struct Player final : public Enumerated<Player> {
    friend class Factory<Player>;

    std::string name;                    /**< The name of the player. */
    Location factory;                    /**< The factory location of the player. */
    std::vector<Dropoff> dropoffs;       /**< The dropoffs this player owns. */
    energy_type energy{};                /**< The amount of energy stockpiled by the player. */
    energy_type factory_energy_deposited{}; /**< The amount of energy deposited at the factory so far. */
    energy_type total_energy_deposited{}; /**< The amount of energy collected so far. */
    const std::string command;           /**< The bot command for the player. */
    id_map<Entity, Location> entities{}; /**< Mapping from entity to location. */
    bool terminated;                     /**< Whether the player was kicked out of the game. */
    bool can_play = true;                /**< Whether the player has sufficient resources remaining. */

    /**
     * Get whether the player has an entity.
     * @param id The entity ID.
     * @return True if the player has the entity, false otherwise.
     */
    bool has_entity(const Entity::id_type &id) const;

    /**
     * Add an entity by ID.
     * @param id The entity ID to add.
     * @param location The location of the entity.
     */
    void add_entity(const Entity::id_type &id, Location location);

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
            Enumerated(id), factory(factory), command(std::move(command)), terminated(false) {}

};

}

namespace std {
template<>
struct hash<hlt::Player> : hash<Enumerated<hlt::Player>> {
};
}

#endif /* PLAYER_H */
