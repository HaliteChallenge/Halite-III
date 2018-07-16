#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <utility>
#include <unordered_map>

#include "Entity.hpp"
#include "Enumerated.hpp"
#include "Location.hpp"

namespace hlt {

/** Representation of a Halite player. */
struct Player : public virtual Enumerated<Player> {
    friend class Factory<Player>;

    /** Type of Player IDs. */
    using id_type = long;

    /** Type of the Entity map of a player, where keys are entity locations. */
    using Entities = std::unordered_map<Location, Entity>;

    std::string name;             /**< The name of the player. */
    std::string command;          /**< The bot command for the player. */
    energy_type energy{};         /**< The amount of energy stockpiled by the player. */
    Location factory;             /**< The factory locations of the player. */
    Entities entities{};          /**< Mapping of location of entity to entity shared ptr */

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
     * Construct Player from ID and command.
     * @param id The player ID.
     * @param command The player bot command.
     */
    Player(Player::id_type id, std::string command) :
            Enumerated(id), command(std::move(command)) {}

    /**
     * Construct Player from ID, name, and command.
     * @param id The player ID.
     * @param name The player name.
     * @param command The player bot command.
     */
    Player(Player::id_type id, std::string name, std::string command) :
            Enumerated(id), name(std::move(name)), command(std::move(command)) {}

    /**
     * Construct Player from ID, name, energy, factory locations, and entities.
     * @param id The player ID.
     * @param name The player name.
     * @param energy The energy.
     * @param factories The factory locations.
     * @param entities The location -> entity mapping.
     */
    Player(Player::id_type id, std::string name, energy_type energy,
           Location factory, Player::Entities entities)
            : Enumerated(id), name(std::move(name)), energy(energy),
              factory(factory), entities(std::move(entities)) {}
};

}

namespace std {
template<>
struct hash<hlt::Player> : hash<Enumerated<hlt::Player>> {};
}

#endif /* PLAYER_H */
