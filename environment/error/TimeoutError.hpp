#ifndef TIMEOUTERROR_HPP
#define TIMEOUTERROR_HPP

#include <chrono>
#include <exception>
#include <string>

#include "BotError.hpp"

/** Thrown when a network event times out. */
struct TimeoutError : public BotError {
    /** The time elapsed. */
    std::chrono::milliseconds time;

    /**
     * Construct TimeoutError from time.
     * @param time The time elapsed.
     */
    explicit TimeoutError(std::chrono::milliseconds time) : time(time) {}

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        return ("Timed out after " + std::to_string(time.count()) + " ms.").c_str();
    }
};

#endif // TIMEOUTERROR_HPP
