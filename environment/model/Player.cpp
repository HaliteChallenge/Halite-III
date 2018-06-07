#include "Player.hpp"

namespace hlt {

void to_json(nlohmann::json &json, const Player &player) {
    json = {{"player_id",    player.player_id},
            {"name",         player.name},
            {"energy",       player.energy},
            {"factory_cell", player.factory_cell},
            {"entities",     player.entities}};
}

void from_json(const nlohmann::json &json, Player &player) {
    player = {json.at("player_id").get<int>(),
              json.at("name").get<std::string>(),
              json.at("energy").get<int>(),
              json.at("factory_cell").get<Cell>(),
              json.at("entities").get<Player::Entities>()};
}

std::ostream &operator<<(std::ostream &os, const Player &player) {
    os << player.player_id << " " << player.entities.size() << " " << player.energy << std::endl;
    for (const auto &entity : player.entities) {
        os << entity;
    }
    return os;
}

bool operator<(const Player &p1, const Player &p2) {
    return p1.player_id < p2.player_id;
}

bool operator==(const Player &p1, const Player &p2) {
    return p1.player_id == p2.player_id;
}

std::ostream &operator<<(std::ostream &os, const std::list<Player> &players) {
    for (const auto &player : players) {
        os << player;
    }
    return os;
}

Player PlayerFactory::new_player(const std::string &name) {
    return Player(next_player++, name);
}

PlayerFactory::PlayerFactory(int next_player) : next_player(next_player) {}

Player::Player(int player_id, const std::string &name) : player_id(player_id), name(name), energy(0),
                                                         factory_cell(Cell(Cell::CellType::Factory, 0, true)) {}

Player::Player(int player_id, const std::string &name, int energy, Cell factory_cell, Player::Entities entities) :
        player_id(player_id), name(name), energy(energy), factory_cell(factory_cell),
        entities(std::move(entities)) {}

}
