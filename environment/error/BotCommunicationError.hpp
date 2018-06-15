#ifndef BOTCOMMUNICATIONERROR_HPP
#define BOTCOMMUNICATIONERROR_HPP

#include <exception>
#include <string>
#include <utility>

/** Thrown when bot communication cannot be decoded. */
class BotCommunicationError : public std::exception {
    /** The bot input. */
    std::string bot_input;
public:
    /**
     * Construct BotCommunicationError from bot input.
     * @param bot_input The bot input.
     */
    explicit BotCommunicationError(std::string bot_input) : bot_input(std::move(bot_input)) {}

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        return ("Failed to decode bot message: \"" + bot_input + "\"").c_str();
    }
};

#endif // BOTCOMMUNICATIONERROR_HPP
