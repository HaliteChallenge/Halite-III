#ifndef TIMEOUTERROR_HPP
#define TIMEOUTERROR_HPP

#include <chrono>
#include <exception>
#include <string>

#include "BotError.hpp"

/** Thrown when a network event times out. */
struct TimeoutError : public BotError {
private:
    std::string result;                   /**< The description buffer. */
public:
    const std::string message;            /**< The message. */
    const std::chrono::milliseconds time; /**< The time elapsed. */
    const std::string remaining_input;    /**< The remaining input. */

    /**
     * Construct TimeoutError from message, time, and remaining input.
     * @param message The message.
     * @param time The time elapsed.
     * @param remaining_input The remaining input.
     */
    explicit TimeoutError(std::string message,
                          std::chrono::milliseconds time,
                          std::string remaining_input = "")
            : message(std::move(message)), time(time), remaining_input(std::move(remaining_input)) {
        std::ostringstream stream;
        stream << "timed out after "
               << std::to_string(time.count())
               << " ms ("
               << this->message
               << ")";
        result = stream.str();
    }

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        return result.c_str();
    }
};

#endif // TIMEOUTERROR_HPP
