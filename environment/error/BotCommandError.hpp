#ifndef BOTCOMMANDERROR_HPP
#define BOTCOMMANDERROR_HPP

#include <stdexcept>
#include <string>

#include "BotError.hpp"

/** Error raised when bot commands are logically invalid. */
class BotCommandError : public BotError {
    /** The error message. */
    std::string message;

public:
    /**
     * Construct BotCommandError from message.
     * @param message The message.
     */
    explicit BotCommandError(const std::string &message) : message(message) {}

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        return message.c_str();
    }
};

#endif // BOTCOMMANDERROR_HPP
