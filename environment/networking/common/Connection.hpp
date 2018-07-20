#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <unordered_map>

#include "NetworkingConfig.hpp"
#include "Player.hpp"

namespace net {

/** Abstract structure containing both ends of pipe to bot. */
class BaseConnection {
protected:
    NetworkingConfig config{}; /** The networking configuration. */

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
     * @return The remaining input.
     */
    virtual std::string read_trailing_input();

    /**
     * Get the error output from this connection.
     * @return The error output.
     */
    virtual std::string get_errors() = 0;

    /** Virtual destructor. */
    virtual ~BaseConnection() = default;
};

/** Container type for connections, exposed to outside users. */
using Connection = std::unique_ptr<BaseConnection>;

/** Generic factory class for connections across platforms. */
template<class OSConnection>
class ConnectionFactory final {
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
