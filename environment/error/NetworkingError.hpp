#ifndef NETWORKINGERROR_HPP
#define NETWORKINGERROR_HPP

#include <cerrno>
#include <chrono>
#include <exception>
#include <string>
#include <system_error>

namespace net {

/** Generic networking exception. */
struct NetworkingError : public std::system_error {
    /**
     * Construct NetworkingError from message.
     * @param message The message.
     */
    explicit NetworkingError(const std::string &message) : std::system_error(errno, std::generic_category(), message) {}
};

}

#endif // NETWORKINGERROR_HPP
