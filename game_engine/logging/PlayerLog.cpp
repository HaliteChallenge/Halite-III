#include "PlayerLog.hpp"

namespace hlt {

/**
 * Write a log message to the player log.
 * @param message The message.
 * @param level The severity level.
 */
void PlayerLog::log(const std::string &message, hlt::PlayerLog::Level level) {
    log_header();
    switch (level) {
    case Level::Warning:
        stream << "warning: ";
        break;
    case Level::Error:
        stream << "error: ";
        break;
    }
    stream << message << std::endl;
}

/**
 * Write a log message to the player log.
 * @param message The message.
 */
void PlayerLog::log(const std::string &message) {
    log_header();
    stream << message << std::endl;
}

/** If a log header for this turn is necessary, print it. */
void PlayerLog::log_header() {
    if (!started_turn) {
        started_turn = true;
        stream << std::endl
               << (turn_number == init_turn ? "Initialization Phase" :
                   turn_number == ended ? "Game End" :
                   "Turn " + std::to_string(turn_number))
               << std::endl
               << "================================================================"
               << std::endl;
    }
}

}
