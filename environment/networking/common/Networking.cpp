#include <sstream>

#include "Command.hpp"
#include "Halite.hpp"
#include "Logging.hpp"

namespace net {

using namespace hlt;

/** Maximum permissible length for user name in characters, truncated past this. */
constexpr auto NAME_MAX_LENGTH = 30;

/**
 * Launch the bot for a player, send the initial game information to the player, and update its name.
 * @param player The player to communicate with.
 */
void Networking::initialize_player(Player &player) {
    Logging::log("Initializing player " + std::to_string(player.player_id) + " with command " + player.command);
    connections[player] = connection_factory.new_connection(player.command);
    std::stringstream message_stream;
    Logging::log("Sending init message to player " + std::to_string(player.player_id));
    // Send the player ID, followed by the game map.
    message_stream << player.player_id << std::endl << game->game_map;
    connections[player]->send_string(message_stream.str());
    Logging::log("Init message sent to player " + std::to_string(player.player_id));
    // Receive a name from the player.
    player.name = connections[player]->get_string().substr(0, NAME_MAX_LENGTH);
    Logging::log("Init message received from player " + std::to_string(player.player_id) + ", name: " + player.name);
}

/**
 * Handle the networking for a single frame, obtaining a command from the player if there is one.
 * Safe to invoke from multiple threads on different players.
 *
 * @param player The player to communicate with.
 * @return The command from the player.
 */
Command Networking::handle_frame(const Player &player) {
    std::stringstream message_stream;
    // Send the turn number, then each player in the game.
    message_stream << game->turn_number << std::endl;
    for (const auto &[_, other_player] : game->players) {
        message_stream << other_player;
    }
    connections[player]->send_string(message_stream.str());
    Logging::log("Turn info sent to player " + std::to_string(player.player_id), Logging::Level::Debug);
    // Get commands from the player.
    std::istringstream command_stream(connections[player]->get_string());
    Command command;
    command_stream >> command;
    Logging::log("Turn info received from player " + std::to_string(player.player_id), Logging::Level::Debug);
    return command;
}

}
