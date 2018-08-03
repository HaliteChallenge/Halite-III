#include "Connection.hpp"
#include "NetworkingError.hpp"
#include "TimeoutError.hpp"

#ifdef _WIN32
#include "WinConnection.hpp"
#else
#include "UnixConnection.hpp"
#endif

namespace net {

/**
 * Read any remaining input from the pipe.
 * @return The remaining input.
 */
std::string BaseConnection::read_trailing_input() {
    std::string result;
    config.ignore_timeout = false;
    config.timeout = std::chrono::milliseconds::zero();
    while (true) {
        try {
            result += get_string();

            // Cap amount of input read
            if (result.size() > MAX_TRAILING_INPUT_READ) {
                result.resize(MAX_TRAILING_INPUT_READ);
                result += "\n...output truncated...\n";
                break;
            }

            result += "\n";
        } catch (const TimeoutError &err) {
            result += err.remaining_input;
            break;
        } catch (const NetworkingError &err) {
            result += err.remaining_input;
            break;
        } catch (...) {
            break;
        }
    }
    return result;
}

/**
 * Create a new connection using a command.
 * @param command The command.
 * @return The new connection.
 */
Connection ConnectionFactory::new_connection(const std::string &command) {
    return std::make_unique<OSConnection>(command, config);
}

/**
 * Add a connection for a player.
 * @param player The player.
 * @param connection The connection.
 */
void Connections::add(hlt::Player::id_type player, Connection connection) {
    map[player] = std::move(connection);
}

/**
 * Get the connection for a player.
 * @param player The player.
 * @return The connection.
 */
Connection &Connections::get(hlt::Player::id_type player) {
    return map[player];
}

/**
 * Remove the connection for a player.
 * @param player The player.
 */
void Connections::remove(hlt::Player::id_type player) {
    map.erase(player);
}

}
