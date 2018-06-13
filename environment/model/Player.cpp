#include "Player.hpp"

#include "nlohmann/json.hpp"

/** The JSON key for player ID. */
constexpr auto JSON_PLAYER_ID = "player_id";
/** The JSON key for name. */
constexpr auto JSON_NAME = "name";
/** The JSON key for energy. */
constexpr auto JSON_ENERGY = "energy";
/** The JSON key for factory location. */
constexpr auto JSON_FACTORY_LOCATION = "factory_location";
/** The JSON key for entities. */
constexpr auto JSON_ENTITIES = "entities";

namespace hlt {

/**
 * Convert a Player to JSON format.
 * @param[out] json The output JSON.
 * @param player The Player to convert.
 */
void to_json(nlohmann::json &json, const Player &player) {
    json = {{JSON_PLAYER_ID,        player.player_id},
            {JSON_NAME,             player.name},
            {JSON_ENERGY,           player.energy},
            {JSON_FACTORY_LOCATION, player.factory_location},
            {JSON_ENTITIES,         player.entities}};
}

/**
 * Convert an encoded Player from JSON format.
 * @param json The JSON.
 * @param[out] player The converted Player.
 */
void from_json(const nlohmann::json &json, Player &player) {
    player = {json.at(JSON_PLAYER_ID),
              json.at(JSON_NAME),
              json.at(JSON_ENERGY),
              json.at(JSON_FACTORY_LOCATION),
              json.at(JSON_ENTITIES)};
}

/**
 * Write a Player to bot serial format.
 * @param ostream The output stream.
 * @param player The Player to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Player &player) {
    // Output player ID, number of entities, and current energy.
    ostream << player.player_id << " " << player.entities.size() << " " << player.energy << std::endl;
    // Output a list of entities.
    for (const auto &entity : player.entities) {
        ostream << entity;
    }
    return ostream;
}

/**
 * Write a list of Players to bot serial format.
 * @param ostream The output stream.
 * @param players The Players to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const std::list<Player> &players) {
    // Output each player one after another.
    for (const auto &player : players) {
        ostream << player;
    }
    return ostream;
}

}
