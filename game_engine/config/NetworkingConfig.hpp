#ifndef NETWORKINGCONFIG_HPP
#define NETWORKINGCONFIG_HPP

#include <chrono>

using namespace std::literals::chrono_literals;

namespace net {

/** Configuration for the networking suite. */
struct NetworkingConfig {
    bool ignore_timeout{};                       /**< Ignore timeouts in network actions. */
    std::chrono::milliseconds timeout = 2000ms;  /**< The networking timeout duration. */
    unsigned long name_max_length = 30;          /**< Maximum permissible length for user name in characters. */
};

}

#endif // NETWORKINGCONFIG_HPP
