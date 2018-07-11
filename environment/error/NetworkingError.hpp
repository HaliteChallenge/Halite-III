#ifndef NETWORKINGERROR_HPP
#define NETWORKINGERROR_HPP

#include <cerrno>
#include <chrono>
#include <exception>
#include <string>
#include <cstring>
#include <sstream>

#include "BotError.hpp"

namespace net {

/** Generic networking exception. */
struct NetworkingError : public BotError {
    std::string message;
    int recorded_errno;
    std::string remaining_input;

    /**
     * Construct NetworkingError from message.
     * @param message The message.
     */
    explicit NetworkingError(const std::string &message)
        : message(message), recorded_errno(errno), remaining_input() {}

    /**
     * Construct NetworkingError from message and remaining input
     * @param message The message.
     * @param remaining_input Any input that the bot sent.
     */
    explicit NetworkingError(const std::string &message, const std::string &remaining_input)
        : message(message), recorded_errno(errno), remaining_input(remaining_input) {}

    const char *what() const noexcept override {
        std::stringstream result;

        result << "Error communicating with bot: "
               << message
               << " errno was: "
               << errno
               << " ("
               << std::strerror(errno)
               << "). Remaining bot input was:"
               << std::endl
               << remaining_input;

        const std::string copy = result.str();
        return strdup(copy.c_str());
    }
};

}

#endif // NETWORKINGERROR_HPP
