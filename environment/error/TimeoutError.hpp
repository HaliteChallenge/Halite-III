#ifndef TIMEOUTERROR_HPP
#define TIMEOUTERROR_HPP

#include <chrono>
#include <exception>
#include <string>

#include "BotError.hpp"

/** Thrown when a network event times out. */
struct TimeoutError : public BotError {
    /** The time elapsed. */
    std::string message;
    std::string remaining_input;
    std::chrono::milliseconds time;

    std::string result;

    /**
     * Construct TimeoutError from time.
     * @param time The time elapsed.
     */
    explicit TimeoutError(const std::string& message,
                          std::chrono::milliseconds time,
                          const std::string& remaining_input)
        : message(message), remaining_input(remaining_input), time(time) {
        result = "Timed out after ";
        result += std::to_string(time.count());
        result += " ms (";
        result += message;
        result += "). Input read:\n";
        result += remaining_input;
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
