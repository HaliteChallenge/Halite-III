#ifndef UNIXCONNECTION_HPP
#define UNIXCONNECTION_HPP

#include <array>
#include <deque>

#include "Connection.hpp"

namespace net {

/** Connections based on UNIX processes and pipes. */
class UnixConnection final : public BaseConnection {
private:
    static constexpr auto READ_BUFFER_SIZE = 256; /**< The buffer size for reading from bots. */
    std::array<char, READ_BUFFER_SIZE> buffer{};  /**< The bot read buffer. */
    std::string current_read;                     /**< Accumulated last read, waiting for a newline. */
    std::deque<std::string> message_queue;        /**< Queue for messages that have not been retrieved. */

    using Process = pid_t;
    using Pipe = int;

    Pipe read_pipe{};  /** The read pipe. */
    Pipe write_pipe{}; /** The write pipe. */
    Pipe error_pipe{}; /** The error pipe. */
    Process process{}; /** The process. */

public:
    /**
     * Initialize a UnixConnection to a new process using a command.
     * Spawns the process, and binds two pipes to it for I/O.
     * @throws NetworkingError if the connection could not be established.
     * @param command The command to launch.
     * @param config The network configuration.
     */
    explicit UnixConnection(const std::string &command, NetworkingConfig config);

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
    std::string get_string() override {
        return get_string(config.timeout);
    }

    /**
     * Get a string from this connection.
     * @param timeout The timeout to use.
     * @return The string read.
     * @throws NetworkingError on error while reading.
     */
    std::string get_string(std::chrono::milliseconds timeout) override;

    /**
     * Get the error output from this connection.
     * @return The error output.
     */
    std::string get_errors() override;

    /** Destroy the UnixConnection, terminating the subprocess if there is one. */
    ~UnixConnection() noexcept override;
};

}

#endif // UNIXCONNECTION_HPP
