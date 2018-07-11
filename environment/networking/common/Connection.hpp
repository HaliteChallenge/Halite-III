#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#endif

#include <unordered_map>

#include "NetworkingConfig.hpp"
#include "Player.hpp"

namespace net {

/** The type of a process. */
#ifdef _WIN32
using Process = HANDLE;
#else
using Process = pid_t;
#endif

/** The type of a pipe endpoint. */
#ifdef _WIN32
using Pipe = HANDLE;
#else
using Pipe = int;
#endif

/** Abstract structure containing both ends of pipe to bot. */
class BaseConnection {
protected:
    Pipe read_pipe{};          /** The read pipe. */
    Pipe write_pipe{};         /** The write pipe. */
    Process process{};         /** The process. */
    NetworkingConfig config{}; /** The networking configuration. */

    BaseConnection() = default;

public:
    /**
     * Send a string along this connection.
     * @param message The string to send.
     * @throws NetworkingError if message could not be sent.
     */
    virtual void send_string(const std::string &message) = 0;

    /**
     * Get a string from this connection.
     * @return The string read.
     * @throws NetworkingError on error while reading.
     */
    virtual std::string get_string() = 0;

    /**
     * Read any remaining input from the pipe.
     */
    virtual std::string read_trailing_input() = 0;

    virtual ~BaseConnection() = default;
};

/** Container type for connections, exposed to outside users. */
using Connection = std::unique_ptr<BaseConnection>;

/** Generic factory class for connections across platforms. */
template<class OSConnection>
class ConnectionFactory {
private:
    /** The networking configuration. */
    NetworkingConfig config;
public:
    /**
     * Construct ConnectionFactory with a network configuration.
     * @param config The network configuration.
     */
    explicit ConnectionFactory(NetworkingConfig &config) : config(config) {};

    /**
     * Create a new connection using a command.
     * @param command The command.
     * @return The new connection.
     */
    Connection new_connection(const std::string &command) {
        return std::make_unique<OSConnection>(command, config);
    }
};

/** Map from players to bot connections. */
using Connections = std::unordered_map<hlt::Player, Connection>;

}

#endif // CONNECTION_HPP
