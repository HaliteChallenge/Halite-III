#include <algorithm>
#include <cassert>
#include <chrono>
#include <csignal>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>

#include "Logging.hpp"
#include "NetworkingError.hpp"
#include "TimeoutError.hpp"
#include "UnixConnection.hpp"

/** Try a call, and throw NetworkingError if it fails. */
#define CHECK(x) do { if ((x) < 0) { throw NetworkingError("Failed to execute " #x); } } while (false)

/** The number of slots in a pipe array. */
constexpr auto PIPE_PAIR = 2;
/** The index of the head of the pipe. */
constexpr auto PIPE_HEAD = 0;
/** The index of the tail of the pipe. */
constexpr auto PIPE_TAIL = 1;

/** Offset added to file descriptor, required by select(2) */
constexpr auto NFDS_OFFSET = 1;

namespace net {

/**
 * Initialize a UnixConnection to a new process using a command.
 * Spawns the process, and binds two pipes to it for I/O.
 * @throws NetworkingError if the connection could not be established.
 * @param command The command to launch.
 * @param config The network configuration.
 */
UnixConnection::UnixConnection(const std::string &command, NetworkingConfig config) {
    this->config = config;
    int write_pipe[PIPE_PAIR];
    int read_pipe[PIPE_PAIR];
    // Ignore SIGPIPE, as we want to detect bot exit gracefully.
    signal(SIGPIPE, SIG_IGN);
    CHECK(pipe(write_pipe));
    CHECK(pipe(read_pipe));
    // Make the write pipe non-blocking
    CHECK(fcntl(write_pipe[PIPE_TAIL], F_SETFL, O_NONBLOCK));

    auto ppid_before_fork = getpid();
    auto pid = fork();
    if (pid == 0) {
        // This is the child
        setpgid(getpid(), getpid());
        if (getppid() != ppid_before_fork) {
            throw NetworkingError("Fork failed");
        }

        // Redirect stdin, stdout, and stderr
        CHECK(dup2(write_pipe[PIPE_HEAD], STDIN_FILENO));
        CHECK(close(write_pipe[PIPE_HEAD]));
        CHECK(dup2(read_pipe[PIPE_TAIL], STDOUT_FILENO));
        CHECK(dup2(read_pipe[PIPE_TAIL], STDERR_FILENO));
        CHECK(close(read_pipe[PIPE_TAIL]));

        // Execute the command
        CHECK(execl("/bin/sh", "sh", "-c", command.c_str(), nullptr));
        // Nothing past here should be run
        throw NetworkingError("Exec failed");
    } else if (pid < 0) {
        throw NetworkingError("Fork failed");
    }
    this->read_pipe = read_pipe[PIPE_HEAD];
    close(read_pipe[PIPE_TAIL]);
    this->write_pipe = write_pipe[PIPE_TAIL];
    close(write_pipe[PIPE_HEAD]);
    process = pid;
}

/**
 * Destroy the UnixConnection, terminating the subprocess if there is one.
 */
UnixConnection::~UnixConnection() noexcept {
    kill(-process, SIGKILL);
}

/**
 * Send a string along this connection.
 * @param message The string to send.
 * @throws NetworkingError if message could not be sent.
 */
void UnixConnection::send_string(const std::string &message) {
    auto length = message.length();
    Logging::log("Sending message with length " + std::to_string(length), Logging::Level::Debug);
    auto message_ptr = message.c_str();
    auto chars_remaining = static_cast<size_t>(length);

    using namespace std::chrono;
    auto initial_time = high_resolution_clock::now();
    while (chars_remaining > 0) {
        if (!config.ignore_timeout) {
            auto current_time = high_resolution_clock::now();
            auto remaining = config.timeout - duration_cast<milliseconds>(current_time - initial_time);
            if (remaining < milliseconds::zero()) {
                throw TimeoutError(config.timeout);
            }
        }
        ssize_t chars_written = write(write_pipe, message_ptr, chars_remaining);
        if (chars_written < 0) {
            switch (errno) {
            case EAGAIN:
                continue;
            default:
                throw NetworkingError("Could not send string");
            }
        }
        message_ptr += chars_written;
        chars_remaining -= chars_written;
    }
}

/**
 * Get a string from this connection.
 * @return The string read.
 * @throws NetworkingError on error while reading.
 */
std::string UnixConnection::get_string() {
    // Try the queue first
    if (!message_queue.empty()) {
        const auto &message = message_queue.front();
        message_queue.pop_front();
        return message;
    }

    fd_set set;
    FD_ZERO(&set);
    assert(read_pipe <= FD_SETSIZE);
    FD_SET(read_pipe, &set);

    using namespace std::chrono;
    auto initial_time = high_resolution_clock::now();
    while (true) {
        // Check if there are bytes in the pipe
        int selection_result;
        if (config.ignore_timeout) {
            selection_result = select(read_pipe + NFDS_OFFSET, &set, nullptr, nullptr, nullptr);
        } else {
            auto current_time = high_resolution_clock::now();
            auto remaining = config.timeout - duration_cast<milliseconds>(current_time - initial_time);
            if (remaining < milliseconds::zero()) {
                throw TimeoutError(config.timeout);
            }
            struct timeval timeout{};
            auto sec = duration_cast<seconds>(remaining);
            timeout.tv_sec = sec.count();
            timeout.tv_usec = (remaining - sec).count();
            selection_result = select(read_pipe + NFDS_OFFSET, &set, nullptr, nullptr, &timeout);
        }
        if (selection_result > 0) {
            // Read from the pipe, as many as we can into the buffer
            auto bytes_read = read(read_pipe, buffer.begin(), buffer.size());
            if (bytes_read < 0) {
                throw NetworkingError("Read failed");
            }
            // Iterator one past the last read character
            auto read_end = buffer.begin() + bytes_read;
            // Identify whether a newline exists in the buffer
            auto newline_pos = std::find(buffer.begin(), read_end, '\n');
            if (newline_pos == read_end) {
                // No newline was found, append the entire read to the result
                current_read += std::string(buffer.begin(), read_end);
                // Keep reading until we find a newline eventually
                continue;
            } else {
                // Newline was found; append the read to the result, up to but not including the newline
                current_read += std::string(buffer.begin(), newline_pos);
                auto result = current_read;

                // There may be more newlines; tokenize the remainder
                auto string_end = ++newline_pos;
                auto read = std::string(string_end, read_end);
                std::stringstream stream(read);
                auto more_messages = false;
                while (std::getline(stream, current_read)) {
                    more_messages = true;
                    message_queue.push_back(current_read);
                }
                if (more_messages && read.back() != '\n') {
                    // The last element is not yet done, don't queue it
                    message_queue.pop_back();
                } else {
                    current_read = "";
                }
                return result;
            }
        }
    }
}

}
