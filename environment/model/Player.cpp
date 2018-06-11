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
    player = {json.at("player_id").get<decltype(player.player_id)>(),
              json.at("name").get<decltype(player.name)>(),
              json.at("energy").get<decltype(player.energy)>(),
              json.at("factory_location").get<decltype(player.factory_location)>(),
              json.at("entities").get<decltype(player.entities)>()};
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
