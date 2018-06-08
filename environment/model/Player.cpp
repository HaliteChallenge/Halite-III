#include "Player.hpp"

namespace hlt {

void to_json(nlohmann::json &json, const Player &player) {
    json = {{"player_id",        player.player_id},
            {"name",             player.name},
            {"energy",           player.energy},
            {"factory_location", player.factory_location},
            {"entities",         player.entities}};
}

void from_json(const nlohmann::json &json, Player &player) {
    player = {json.at("player_id").get<long>(),
              json.at("name").get<std::string>(),
              json.at("energy").get<long>(),
              json.at("factory_location").get<Location>(),
              json.at("entities").get<Player::Entities>()};
}

std::ostream &operator<<(std::ostream &ostream, const Player &player) {
    ostream << player.player_id << " " << player.entities.size() << " " << player.energy << std::endl;
    for (const auto &entity : player.entities) {
        ostream << entity;
    }
    return ostream;
}

std::ostream &operator<<(std::ostream &ostream, const std::list<Player> &players) {
    for (const auto &player : players) {
        ostream << player;
    }
    return ostream;
}

}
