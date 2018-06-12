#ifndef NETWORKINGCONFIG_HPP
#define NETWORKINGCONFIG_HPP

#include <chrono>

namespace net {

/** Configuration for the networking suite. */
struct NetworkingConfig {
    bool ignore_timeout;               /**< Ignore timeouts in network actions. */
    std::chrono::milliseconds timeout; /**< If not ignored, time out after this duration. */
};

}

#endif // NETWORKINGCONFIG_HPP
