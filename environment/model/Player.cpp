#include "Player.hpp"

#include "nlohmann/json.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, player.x}

/** Get a field from JSON. */
#define FIELD_FROM_JSON(x) json.at(#x)

namespace hlt {

/**
 * Convert a Player to JSON format.
 * @param[out] json The output JSON.
 * @param player The Player to convert.
 */
void to_json(nlohmann::json &json, const Player &player) {
    json = {FIELD_TO_JSON(player_id),
            FIELD_TO_JSON(name),
            FIELD_TO_JSON(energy),
            FIELD_TO_JSON(factory_location),
            FIELD_TO_JSON(entities)};
}

/**
 * Convert an encoded Player from JSON format.
 * @param json The JSON.
 * @param[out] player The converted Player.
 */
void from_json(const nlohmann::json &json, Player &player) {
    player = {FIELD_FROM_JSON(player_id),
              FIELD_FROM_JSON(name),
              FIELD_FROM_JSON(energy),
              FIELD_FROM_JSON(factory_location),
              FIELD_FROM_JSON(entities)};
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
