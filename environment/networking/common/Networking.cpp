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
    // Send the number of players and player ID
    message_stream << game->players.size()
                   << " " << player.player_id << std::endl;
    // Send each player's ID and factory location
    for (const auto &[player_id, other_player] : game->players) {
        message_stream << player_id
                       << " " << other_player.factory_location.first
                       << " " << other_player.factory_location.second
                       << std::endl;
    }
    // Send the map
    message_stream << game->game_map;
    connections[player]->send_string(message_stream.str());
    Logging::log("Init message sent to player " + std::to_string(player.player_id));
    // Receive a name from the player.
    player.name = connections[player]->get_string().substr(0, NAME_MAX_LENGTH);
    Logging::log("Init message received from player " + std::to_string(player.player_id) + ", name: " + player.name);
}

/**
 * Handle the networking for a single frame, obtaining a command from the player if there is one.
 * @param player The player to communicate with.
 * @param[out] command The command from the player.
 */
void Networking::handle_frame(const Player &player, Command &command) {
    std::stringstream message_stream;
    // Send the turn number, then each player in the game.
    message_stream << game->turn_number << std::endl;
    for (const auto &player_entry : game->players) {
        message_stream << player_entry.second;
    }
    connections[player]->send_string(message_stream.str());
    Logging::log("Turn info sent to player " + std::to_string(player.player_id), Logging::Level::Debug);
    // Get commands from the player.
    std::istringstream command_stream(connections[player]->get_string());
    command_stream >> command;
    Logging::log("Turn info received from player " + std::to_string(player.player_id), Logging::Level::Debug);
}

}
