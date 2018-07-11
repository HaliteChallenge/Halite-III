#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <utility>
#include <unordered_map>

#include "Location.hpp"

namespace hlt {

struct PlayerEntity;

/** Representation of a Halite player. */
struct Player {
    friend class PlayerFactory;

    /** Type of Player IDs. */
    using id_type = long;

    /** Type of the Entity map of a player, where keys are entity locations. */
    using Entities = std::unordered_map<Location, std::shared_ptr<PlayerEntity>>;

    Player::id_type player_id{};      /**< The unique ID of the player. */
    std::string name;                 /**< The name of the player. */
    std::string command;              /**< The bot command for the player. */
    energy_type energy{};             /**< The amount of energy stockpiled by the player. */
    std::vector<Location> factories;  /**< The factory locations of the player. */
    Entities entities{};              /**< Mapping of location of entity to entity shared ptr */
    std::string error_log;            /**< A log of errors this player encounters. */
    bool alive;                       /**< Whether the player is still in the game. */
    bool crashed;                     /**< Whether the player was kicked out of the game. */

    /**
     * Find an entity by location.
     * @param location The location to search.
     * @return The entity there, or null if not found.
     */
    std::shared_ptr<PlayerEntity> find_entity(const Location &location) const;

    /**
     * Add a new entity by location. No entity must exist at that location.
     * @param location The location for the entity.
     * @param entity The entity to add.
     */
    void add_entity(const Location &location, std::shared_ptr<PlayerEntity> entity);

    /**
     * Remove an entity by location.
     * @param location The location of the entity.
     * @return The entity there.
     */
    std::shared_ptr<PlayerEntity> remove_entity(const Location &location);

    /**
     * Add a section to this player's error log.
     */
    void log_error_section(const std::string& section_name);

    /**
     * Add a line to this player's error log.
     */
    void log_error(const std::string& text);

    /**
     * Convert a Player to JSON format.
     * @param[out] json The output JSON.
     * @param player The Player to convert.
     */
    friend void to_json(nlohmann::json &json, const Player &player);

    /** Test two Player instances for equality. */
    bool operator==(const Player &other) const { return player_id == other.player_id; }

    /** Order two Player instances by ID. */
    bool operator<(const Player &other) const { return player_id < other.player_id; }

    /**
     * Write a Player to bot serial format.
     * @param ostream The output stream.
     * @param player The Player to write.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const Player &player);

    /** Default constructor required by std::map. */
    Player() = default;

private:
    /**
     * Construct Player from ID, name, and command.
     * @param player_id The player ID.
     * @param name The player name.
     * @param command The player bot command.
     */
    Player(Player::id_type player_id, std::string name, std::string command) :
            player_id(player_id), name(std::move(name)), command(std::move(command)),
            error_log(), alive(true), crashed(false) {}

    /**
     * Construct Player from ID, name, energy, factory locations, and entities.
     * @param player_id The player ID.
     * @param name The player name.
     * @param energy The energy.
     * @param factories The factory locations.
     * @param entities The location -> entity mapping.
     */
    Player(Player::id_type player_id, std::string name, energy_type energy,
           const std::vector<Location> &factories, Player::Entities entities)
            : player_id(player_id), name(std::move(name)), energy(energy),
              factories(factories), entities(std::move(entities)), error_log(""),
              alive(true), crashed(false) {}
};

/** Factory which produces Players. */
class PlayerFactory {
    /** The next player to allocate, starting from zero. */
    Player::id_type next_player{};

public:
    /**
     * Make a new player with default name (which is ID), and a command.
     * @param command The bot command.
     * @return The new player.
     */
    Player new_player(std::string command) {
        auto player_id = next_player++;
        return {player_id, std::to_string(player_id), std::move(command)};
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
