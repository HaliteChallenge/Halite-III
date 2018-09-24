#ifndef NETWORKING_H
#define NETWORKING_H

#include <map>
#include <mutex>
#include <unordered_map>
#include <utility>

#include "Command.hpp"
#include "Connection.hpp"
#include "NetworkingConfig.hpp"
#include "Player.hpp"

#ifdef _WIN32
#include "WinConnection.hpp"
#else

#include "UnixConnection.hpp"

#endif

namespace hlt {
class Halite;
}

namespace net {

/** Networking support suite for Halite. */
class Networking final {
private:
    ConnectionFactory connection_factory; /**< The platform-specific connection factory. */
    Connections connections;      /**< The current network connections. */
    NetworkingConfig config;      /**< The networking configuration. */
    hlt::Halite &game;            /**< The current game. */

public:
    /**
     * Launch the bot for a player and register their connection.
     *
     * @param player The player.
     */
    void connect_player(hlt::Player &player);

    /**
     * Send the initial game information to the player, and update its name.
     * Safe to invoke from multiple threads on different players.
     *
     * @param player The player to communicate with.
     */
    void initialize_player(hlt::Player &player);

    /**
     * Kill a player connection.
     *
     * @param player The player whose connection to end.
     */
    void kill_player(const hlt::Player &player);

    /**
     * Handle the networking for a single frame, obtaining commands from the player if there are any.
     * Safe to invoke from multiple threads on different players.
     *
     * @param player The player to communicate with.
     * @return The commands from the player.
     */
    std::vector<std::unique_ptr<hlt::Command>> handle_frame(hlt::Player &player);

    /**
     * Handle a player communication error.
     * @param player The player ID.
     * @param received_input The current received input from the player.
     */
    void handle_player_error(hlt::Player::id_type player, std::string received_input = "");

    /**
     * Initialize Networking from configuration and Halite game.
     * @param config The configuration.
     * @param game The Halite game.
     */
    explicit Networking(NetworkingConfig config, hlt::Halite &game) :
            connection_factory(config), config(config), game(game) {};
};

}

#endif
