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

/** The type of platform-specific connections. */
#ifdef _WIN32
using OSConnection = WinConnection;
#else
using OSConnection = UnixConnection;
#endif

/** Networking support suite for Halite. */
class Networking final {
private:
    ConnectionFactory<OSConnection> connection_factory; /**< The platform-specific connection factory. */
    Connections connections{};    /**< The current network connections. */
    NetworkingConfig config;      /**< The networking configuration. */
    const hlt::Halite &game;      /**< The current game. */
    std::mutex connections_mutex; /**< Mutex used to protect the connections map. */

public:
    /**
     * Launch the bot for a player, send the initial game information to the player, and update its name.
     * Safe to invoke from multiple threads on different players.
     *
     * @param player The player to communicate with.
     */
    void initialize_player(hlt::Player &player);

    /**
     * Handle the networking for a single frame, obtaining commands from the player if there are any.
     * Safe to invoke from multiple threads on different players.
     *
     * @param player The player to communicate with.
     * @return The commands from the player.
     */
    std::vector<std::unique_ptr<hlt::Command>> handle_frame(const hlt::Player &player);

    /**
     * Initialize Networking from configuration and Halite game.
     * @param config The configuration.
     * @param game The Halite game.
     */
    explicit Networking(NetworkingConfig config, const hlt::Halite &game) :
            connection_factory(config), config(config), game(game) {};
};

}

#endif
