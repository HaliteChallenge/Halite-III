#ifndef NETWORKINGERROR_HPP
#define NETWORKINGERROR_HPP

#include <exception>
#include <string>
#include <cerrno>

namespace net {

/** Generic networking exception. */
struct NetworkingError : public std::exception {
    std::string message{};  /**< The exception message. */
    std::string aux_info{}; /**< Auxiliary information about the networking issue. */
    int _errno{};           /**< The captured errno. */

    /**
     * Construct NetworkingError from message.
     * @param message The message.
     */
    explicit NetworkingError(std::string message) : message(std::move(message)), _errno(errno) {}

    /**
     * Construct NetworkingError from two messages.
     * @param message The main message.
     * @param aux_info The auxiliary info.
     */
    explicit NetworkingError(std::string message, std::string aux_info) :
            message(std::move(message)), aux_info(std::move(aux_info)), _errno(errno) {}

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        return (message + " " + aux_info + " [" + std::strerror(_errno) + "]").c_str();
    }
};

}

#endif // NETWORKINGERROR_HPP
