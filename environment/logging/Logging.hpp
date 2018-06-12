#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <mutex>
#include <string>

/**
 * Logging framework for Halite.
 *
 * Due to our desire for fewer dependencies, we do not use a library such as Boost.
 * The number of dependencies makes such a library difficult to incorporate, and our needs
 * are quite basic compared to the library features.
 * Logging is currently based on console (standard error) output.
 *
 * We improve upon a previous approach to logging by centralizing the responsibility
 * in this class, whose static members control logging for the Halite engine.
 */
struct Logging {
    /** The available log severity levels, from low to high severity. */
    enum class Level {
        Debug = 0,   /**< For development and debugging information that should generally be hidden. */
        Info = 1,    /**< For informational messages that can be suppressed for quieter output. */
        Warning = 2, /**< For concerning but non-fatal messages during engine or bot operation. */
        Error = 3,   /**< For important and potentially fatal messages. */
    };

    /** The number of levels. */
    static constexpr int NUM_LEVELS = 4;

    /**
     * The current logging verbosity level.
     * Messages of less severity will not be printed to console.
     */
    static Level level;

    /**
     * Set the logging verbosity level.
     * @param level The new verbosity level.
     */
    static void set_level(Level level);

    /**
     * Log a message to console.
     * @param message The message to log.
     * @param level The severity of the message, defaulting to Info.
     */
    static void log(const std::string &message, Level level = Level::Info);

private:
    /**
     * Mutex used to protect standard error,
     * where concurrent log messages are otherwise interleaved.
     */
    static std::mutex cerr_mutex;
};

#endif // LOGGING_HPP
