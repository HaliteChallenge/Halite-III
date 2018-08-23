#ifndef PLAYERLOG_HPP
#define PLAYERLOG_HPP

#include <fstream>

#include "Player.hpp"

namespace hlt {

/** Log associated with a player. */
class PlayerLog {
public:
    /** Sentinel turn number for the initialization phase. */
    static constexpr long init_turn = -1;
    /** Sentinel turn number for after the game ended. */
    static constexpr long ended = -2;

private:
    std::ostringstream stream;    /**< The log output stream. */
    long turn_number = init_turn; /**< The turn number. */
    bool started_turn = false;    /**< Whether this turn has already logged events. */

    /** If a log header for this turn is necessary, print it. */
    void log_header();

public:
    /** The severity level of a message in a player log. */
    enum class Level {
        Warning, Error
    };

    /**
     * Set the turn number.
     * @param turn_number The turn number.
     */
    void set_turn_number(long turn_number) {
        this->turn_number = turn_number;
        this->started_turn = false;
    }

    /**
     * Write a log message to the player log.
     * @param message The message.
     * @param level The severity level.
     */
    void log(const std::string &message, Level level);

    /**
     * Write a log message to the player log.
     * @param message The message.
     */
    void log(const std::string &message);

    /**
     * Get the error output as a string.
     * @return The error string.
     */
    std::string str() { return stream.str(); }
};

class PlayerLogs {
private:
    id_map<Player, PlayerLog> logs;
public:

    /**
     * Set the turn number.
     * @param turn_number The turn number.
     */
    void set_turn_number(long turn_number) {
        for (auto &[_, log] : logs) {
            log.set_turn_number(turn_number);
        }
    }

    /**
     * Write a log message to a player log.
     * @param player The player.
     * @param message The message.
     * @param level The severity level.
     */
    void log(Player::id_type player, const std::string &message, PlayerLog::Level level) {
        logs.find(player)->second.log(message, level);
    }

    /**
     * Write a log message to a player log.
     * @param player The player.
     * @param message The message.
     */
    void log(Player::id_type player, const std::string &message) {
        logs.find(player)->second.log(message);
    }

    /**
     * Get the error output as a string.
     * @param player The player.
     * @return The error string.
     */
    std::string str(Player::id_type player) { return logs.find(player)->second.str(); }

    /**
     * Add a player to the logs.
     * @param player The player.
     */
    void add(Player::id_type player) {
        logs.emplace(player, PlayerLog());
    }
};

}

#endif // PLAYERLOG_HPP
