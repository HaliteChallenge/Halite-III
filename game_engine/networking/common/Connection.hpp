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

    /** The maximum bytes that will be read after a bot dies. */
    constexpr static auto MAX_TRAILING_INPUT_READ = 4 * 1024;

public:
    /**
     * Send a string along this connection.
     * @param message The string to send.
     * @throws NetworkingError if message could not be sent.
     */
    virtual void send_string(const std::string &message) = 0;

    /**
     * Get a string from this connection with configured timeout.
     * @return The string read.
     * @throws NetworkingError on error while reading.
     */
    virtual std::string get_string() = 0;

    /**
     * Get a string from this connection.
     * @param timeout The timeout to use.
     * @return The string read.
     * @throws NetworkingError on error while reading.
     */
    virtual std::string get_string(std::chrono::milliseconds timeout) = 0;

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

/** The type of platform-specific connections. */
#ifdef _WIN32
class WinConnection;
using OSConnection = WinConnection;
#else
class UnixConnection;
using OSConnection = UnixConnection;
#endif

/** Container type for connections, exposed to outside users. */
using Connection = std::unique_ptr<OSConnection>;

/** Factory class for connections. */
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
    Connection new_connection(const std::string &command);
};

/** Map from players to bot connections. */
class Connections {
    id_map<hlt::Player, Connection> map; /**< The connections map. */

public:
    /**
     * Add a connection for a player.
     * @param player The player.
     * @param connection The connection.
     */
    void add(hlt::Player::id_type player, Connection connection);

    /**
     * Get the connection for a player.
     * @param player The player.
     * @return The connection.
     */
    Connection &get(hlt::Player::id_type player);

    /**
     * Remove the connection for a player.
     * @param player The player.
     */
    void remove(hlt::Player::id_type player);
};

}

#endif // CONNECTION_HPP
