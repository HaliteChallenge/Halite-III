#include <iostream>

#include "Logging.hpp"

#include "ansi.hpp"

std::mutex Logging::cerr_mutex;

long Logging::turn_number = -1;

/** The names of the log levels, indexed by level. */
constexpr char const *level_names[] = {"debug", "info", "warn", "error"};

/** The colors of the log levels. */
constexpr char const *level_colors[] = {ansi::reset, ansi::cyan, ansi::bold_yellow, ansi::bold_red};

/** We default to the Info level of logging so that the user may see feedback. */
Logging::Level Logging::level = Logging::Level::Info;

/** Enable logging by default. */
bool Logging::enabled = true;

/**
 * Set the logging verbosity level.
 * @param level The new verbosity level.
 */
void Logging::set_level(Logging::Level level) {
    Logging::level = level;
}

/*
 * Set whether logging is enabled.
 * @param enabled Whether logging is enabled.
 */
void Logging::set_enabled(bool enabled) {
    Logging::enabled = enabled;
}

/**
 * Immediately log a message, without level checking.
 * @param message The message to log.
 * @param level The log level.
 */
void Logging::_log(const std::string &message, Level level) {
    std::lock_guard<std::mutex> guard(Logging::cerr_mutex);
    auto level_num = static_cast<int>(level);
    std::cerr << "["
              << level_colors[level_num]
              << level_names[level_num]
              << ansi::reset
              << "] ";
    if (turn_number >= 0) {
        std::cerr << "[" << turn_number << "] ";
    }
    std::cerr << message << std::endl;
}

/**
 * Set the turn number.
 * @param turn_number The turn number.
 */
void Logging::set_turn_number(unsigned long turn_number) {
    Logging::turn_number = static_cast<long>(turn_number);
}

/** Remove the turn number. */
void Logging::remove_turn_number() {
    Logging::turn_number = -1;
}
