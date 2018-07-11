#ifndef BOTCOMMUNICATIONERROR_HPP
#define BOTCOMMUNICATIONERROR_HPP

#include <exception>
#include <string>
#include <iostream>
#include <utility>

#include "BotError.hpp"

/** Thrown when bot communication cannot be decoded. */
class BotCommunicationError : public BotError {
    /** The bot input. */
    std::string bot_input;
    std::iostream::pos_type position;

public:
    /**
     * Construct BotCommunicationError from bot input.
     * @param bot_input The bot input.
     */
    explicit BotCommunicationError(std::string bot_input)
        : bot_input(std::move(bot_input)), position(-1) {}

    explicit BotCommunicationError(std::string bot_input,
                                   std::iostream::pos_type position)
        : bot_input(std::move(bot_input)), position(position) {}

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        if (position >= 0) {
            return ("Failed to decode bot message: \"" + bot_input +
                    "\" (at position " + std::to_string(position) + ")").c_str();
        }
        return ("Failed to decode bot message: \"" + bot_input + "\"").c_str();
    }
};

#endif // BOTCOMMUNICATIONERROR_HPP
