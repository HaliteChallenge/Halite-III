#ifndef BOTCOMMUNICATIONERROR_HPP
#define BOTCOMMUNICATIONERROR_HPP

#include <exception>
#include <string>
#include <iostream>
#include <utility>

#include "BotError.hpp"

/** Thrown when bot communication cannot be decoded. */
class BotCommunicationError : public BotError {
    /** The message buffer. */
    std::string buffer;
public:
    /**
     * Construct BotCommunicationError from bot input.
     * @param bot_input The bot input.
     */
    explicit BotCommunicationError(const std::string &bot_input) {
        buffer = "failed to decode bot message: \"" + bot_input + "\"";
    }

    /**
     * Construct BotCommunicationError from bot input and position.
     * @param bot_input The bot input.
     * @param position The decoding position.
     */
    explicit BotCommunicationError(const std::string &bot_input,
                                   std::iostream::pos_type position) {
        buffer = "failed to decode bot message: \"" + bot_input +
                 "\" (at position " + std::to_string(position) + ")";
    }

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        return buffer.c_str();
    }
};

#endif // BOTCOMMUNICATIONERROR_HPP
