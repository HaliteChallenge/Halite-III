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

    /** Sentinel turn number for the initialization phase. */
    static constexpr long init_turn = -1;

    /** Sentinel turn number for after the game ended. */
    static constexpr long ended = -2;

    /** The current turn number. */
    static long turn_number;

    /** Whether logging is enabled. */
    static bool enabled;

    /**
     * Set the logging verbosity level.
     * @param level The new verbosity level.
     */
    static void set_level(Level level);

    /*
     * Set whether logging is enabled.
     * @param enabled Whether logging is enabled.
     */
    static void set_enabled(bool enabled);

    /**
     * Set the turn number.
     * @param turn_number The turn number.
     */
    static void set_turn_number(long turn_number);

    /** Remove the turn number. */
    static void remove_turn_number();

    /**
     * Log a suspended (possibly not yet evaluated) message to console.
     * @tparam Message The type of the suspended message.
     * @param message The suspended message or callable.
     * @param level The severity of the message, defaulting to Info.
     */
    template<class Message>
    static void log(const Message &message, Level level = Level::Info) {
        if (!Logging::enabled) {
            return;
        }
        auto level_num = static_cast<int>(level);
        if (level_num >= static_cast<int>(Logging::level)) {
            if constexpr (std::is_convertible_v<Message, std::string>) {
                _log(message, level);
            } else {
                _log(message(), level);
            }
        }
    }

private:
    /**
     * Mutex used to protect standard error,
     * where concurrent log messages are otherwise interleaved.
     */
    static std::mutex cerr_mutex;

    /**
     * Immediately log a message, without level checking.
     * @param message The message to log.
     * @param level The log level.
     */
    static void _log(const std::string &message, Level level);
};

#endif // LOGGING_HPP
