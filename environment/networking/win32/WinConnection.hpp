#ifndef WINCONNECTION_HPP
#define WINCONNECTION_HPP

#include "Connection.hpp"

namespace net {

/** Connections based on Windows processes and pipes. */
class WinConnection : public BaseConnection {
public:
    /**
     * Initialize a WinConnection to a new process using a command.
     * Spawns the process, and binds two pipes to it for I/O.
     * @throws NetworkingError if the connection could not be established.
     * @param command The command to launch.
     * @param config The network configuration.
     */
    explicit WinConnection(const std::string &command, NetworkingConfig config);

    /**
     * Send a string along this connection.
     * @param message The string to send.
     * @throws NetworkingError if message could not be sent.
     */
    void send_string(const std::string &message) override;

    /**
     * Get a string from this connection.
     * @return The string read.
     * @throws NetworkingError on error while reading.
     */
    std::string get_string() override;

    /** Destroy the WinConnection, terminating the subprocess if there is one. */
    ~WinConnection() noexcept override;
};

}

#endif // WINCONNECTION_HPP
