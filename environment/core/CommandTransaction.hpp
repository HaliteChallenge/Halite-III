#ifndef COMMANDTRANSACTION_HPP
#define COMMANDTRANSACTION_HPP

#include "Location.hpp"

namespace hlt {

struct Player;

class Map;

/** Transactions that execute a series of player commands atomically. */
class CommandTransaction {
    std::vector<std::tuple<Location, Location>> move_commands{}; /** The move command buffer. */
    std::vector<std::tuple<Location, energy_type>> spawn_commands{}; /** The spawn command buffer. */
    Map &_map;                                              /** Mutable map reference. */
    Player &_player;                                        /** Mutable player reference. */

public:
    const Map &map;         /** The game map to update. */
    const Player &player;   /** The player executing the commands. */

    /**
     * Construct CommandTransaction from Map and Player.
     * @param map The Map to update.
     * @param player The Player.
     */
    explicit CommandTransaction(Map &map, Player &player) : _map(map), _player(player), map(map), player(player) {}

    /**
     * Attempt to commit the transaction.
     * @return True if the commit succeeded.
     */
    bool commit_moves();

    /**
     * Attempt to commit the transaction.
     * @return True if the commit succeeded.
     */
    bool commit_spawn(std::vector<std::tuple<Location, energy_type>> & spawns);

    /**
     * Add an entity move to the transaction.
     *
     * @param from The source location of the entity.
     * @param to The destination location of the entity.
     */
    void move_entity(const Location &from, const Location &to) {
        move_commands.emplace_back(from, to);
    }

    /**
     * Add an entity spawn to the transaction.
     *
     * @param factory The factory location to spawn from.
     * @param energy The amount of energy to spawn with.
     */
    void spawn_entity(const Location & factory, const energy_type & energy) {
        spawn_commands.emplace_back(factory, energy);
    }
};

}

#endif // COMMANDTRANSACTION_HPP
