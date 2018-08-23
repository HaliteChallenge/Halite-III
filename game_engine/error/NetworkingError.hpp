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
private:
    std::string result;                /** The description buffer. */
public:
    const std::string message;         /** The message. */
    const std::string remaining_input; /**< The remaining input. */
    const int recorded_errno;          /** The captured errno. */

    /**
     * Construct NetworkingError from message and remaining input.
     * @param message The message.
     * @param remaining_input The remaining input.
     */
    explicit NetworkingError(std::string message, std::string remaining_input = "")
            : message(std::move(message)), remaining_input(std::move(remaining_input)), recorded_errno(errno) {
        std::stringstream stream;
        stream << "communication error with bot: "
               << this->message
               << ", errno was: "
               << recorded_errno
               << " ("
               << std::strerror(recorded_errno)
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

}

#endif // NETWORKINGERROR_HPP
