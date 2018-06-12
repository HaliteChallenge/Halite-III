#include <iostream>

#include "Logging.hpp"

std::mutex Logging::cerr_mutex;

/** The names of the log levels, indexed by level. */
constexpr char const *level_names[] = {"DEBUG", "INFO", "WARNING", "ERROR"};

/** We default to the Info level of logging so that the user may see feedback. */
Logging::Level Logging::level = Logging::Level::Info;

/**
 * Set the logging verbosity level.
 * @param level The new verbosity level.
 */
void Logging::set_level(Logging::Level level) {
    Logging::level = level;
}

/**
 * Log a message to console.
 * @param message The message to log.
 * @param level The severity of the message, defaulting to Info.
 */
void Logging::log(const std::string &message, Logging::Level level) {
    std::lock_guard<std::mutex> guard(Logging::cerr_mutex);
    // Only log at sufficiently high levels, otherwise suppress.
    auto level_num = static_cast<int>(level);
    if (level_num >= static_cast<int>(Logging::level)) {
        std::cerr << "[" << level_names[level_num] << "] " << message << std::endl;
    }
}
