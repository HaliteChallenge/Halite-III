#ifndef PLAYER_H
#define PLAYER_H

#include <list>
#include <string>
#include <utility>

#include "Cell.hpp"
#include "Entity.hpp"

namespace hlt {

/** Representation of a Halite player. */
struct Player {
    friend class PlayerFactory;

    using Entities = std::list<Entity>;

    int player_id; /**< The unique ID of the player. */
    std::string name; /**< The name of the player. */
    int energy; /**< The amount of energy stockpiled by the player. */
    Cell factory_cell; /**< The factory cell of the player. */
    Entities entities; /**< The entities owned by the player. */

    friend void to_json(nlohmann::json &json, const Player &player);

    friend void from_json(const nlohmann::json &json, Player &player);

    friend std::ostream &operator<<(std::ostream &os, const Player &player);

    friend std::ostream &operator<<(std::ostream &os, const std::list<Player> &players);

private:
    Player(int player_id, const std::string &name);

    Player(int player_id, const std::string &name, int energy, Cell factory_cell, Entities entities);
};

bool operator==(const Player &p1, const Player &p2);

bool operator<(const Player &p1, const Player &p2);

/** Factory which produces Players. */
class PlayerFactory {
    /** The next ID to allocate. */
    int next_player;

public:
    /** Get a new player. */
    Player new_player(const std::string &name);

    explicit PlayerFactory(int next_player = 0);
};

}

namespace std {
template<>
struct hash<hlt::Player> {
    size_t operator()(const hlt::Player &p) const {
        return (size_t) p.player_id;
    }
};
}
#endif /* PLAYER_H */
