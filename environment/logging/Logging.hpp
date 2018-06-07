#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>

/** Logging framework for Halite. */
struct Logging {
    /** The available log levels. */
    enum class Level {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
    };

    static Level level;

    static void set_level(Level level);

    static void log(Level level, const std::string &msg);

private:
    static std::mutex cerr_mutex;
};

#endif // LOGGING_HPP
