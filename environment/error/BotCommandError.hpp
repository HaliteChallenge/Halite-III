#ifndef BOTCOMMANDERROR_HPP
#define BOTCOMMANDERROR_HPP

#include <stdexcept>
#include <string>

/** Error raised when bot commands are logically invalid. */
class BotCommandError : public std::runtime_error {
public:
    /**
     * Construct BotCommandError from message.
     * @param message The message.
     */
    explicit BotCommandError(const std::string &message) : std::runtime_error(message) {}
};

#endif // BOTCOMMANDERROR_HPP
