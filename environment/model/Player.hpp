#ifndef PLAYER_H
#define PLAYER_H

#include <list>
#include <string>

#include "Entity.hpp"

namespace hlt {

/** Representation of a Halite player. */
struct Player {
    friend class PlayerFactory;

    /** Type of the Entity list of a player. */
    using Entities = std::list<Entity>;

    long player_id;               /**< The unique ID of the player. */
    std::string name;             /**< The name of the player. */
    long energy{};                /**< The amount of energy stockpiled by the player. */
    Location factory_location{};  /**< The factory location of the player. */
    Entities entities;            /**< The entities owned by the player. */

    /**
     * Convert a Player to JSON format.
     * @param[out] json The output JSON.
     * @param player The Player to convert.
     */
    friend void to_json(nlohmann::json &json, const Player &player);

    /**
     * Convert an encoded Player from JSON format.
     * @param json The JSON.
     * @param[out] player The converted Player.
     */
    friend void from_json(const nlohmann::json &json, Player &player);

    /** Test two Player instances for equality. */
    bool operator==(const Player &other) const { return player_id == other.player_id; }

    /** Order two Entity instances by ID. */
    bool operator<(const Player &other) const { return player_id < other.player_id; }

    /**
     * Write a Player to bot serial format.
     * @param ostream The output stream.
     * @param player The Player to write.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const Player &player);

private:
    /**
     * Construct Player from ID and name.
     * @param player_id The player ID.
     * @param name The player name.
     */
    Player(long player_id, std::string name) : player_id(player_id), name(std::move(name)) {}

    /**
     * Construct Player from ID, name, energy, factory location, and entities.
     * @param player_id The player ID.
     * @param name The player name.
     * @param energy The energy.
     * @param factory_location The factory location.
     * @param entities The entities.
     */
    Player(long player_id, std::string name, long energy, Location factory_location, Player::Entities entities)
            : player_id(player_id), name(std::move(name)), energy(energy), factory_location(factory_location),
              entities(std::move(entities)) {}
};

/**
 * Write a list of Players to bot serial format.
 * @param ostream The output stream.
 * @param players The Players to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const std::list<Player> &players);

/** Factory which produces Players. */
class PlayerFactory {
    /** The next player to allocate, starting from zero. */
    long next_player{};

public:
    /**
     * Make a new player.
     * @param name The name of the player.
     * @return The new player.
     */
    Player new_player(const std::string &name) {
        return {next_player++, name};
    }

    PlayerFactory() = default;
};

}

namespace std {
template<>
struct hash<hlt::Player> {
    size_t operator()(const hlt::Player &player) const {
        return (size_t) player.player_id;
    }
};
}
#endif /* PLAYER_H */
