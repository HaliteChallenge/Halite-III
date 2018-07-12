#ifndef COMMANDTRANSACTION_HPP
#define COMMANDTRANSACTION_HPP

#include <functional>

#include "Command.hpp"
#include "Location.hpp"
#include "GameEvent.hpp"

namespace hlt {

struct Player;

class Map;

/** Transactions that execute a series of player commands atomically. */
class CommandTransaction {
    std::function<void(GameEvent)> callback{}; /** Event callback. */

public:
    const Map &map;       /** The game map to update. */
    const Player &player; /** The player executing the commands. */

    std::vector<DumpCommand> dump_commands{};           /** The dump command buffer. */
    std::vector<ConstructCommand> construct_commands{}; /** The construct command buffer. */
    std::vector<MoveCommand> move_commands{};           /** The move command buffer. */
    std::vector<SpawnCommand> spawn_commands{};         /** The spawn command buffer. */

    /**
     * Construct CommandTransaction from Map and Player.
     * @param map The Map to update.
     * @param player The Player.
     */
    explicit CommandTransaction(Map &map, Player &player) : map(map), player(player) {}

    /**
     * Set a callback for GameEvents generated during the transaction commit.
     * @param callback The callback to set.
     */
    void set_callback(std::function<void(GameEvent)> &&callback) { this->callback = callback; }

    /**
     * Check whether the commit can succeed without executing it.
     * @return True if the commit can succeed.
     */
    bool check() const;

    /**
     * Attempt to commit the transaction.
     * @return True if the commit succeeded.
     */
    bool commit();
};

}

#endif // COMMANDTRANSACTION_HPP
