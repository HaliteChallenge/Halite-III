#include <sstream>

#include "Command.hpp"
#include "Halite.hpp"
#include "Logging.hpp"
#include "error/BotError.hpp"
#include "error/NetworkingError.hpp"

namespace net {

using namespace hlt;

/** Maximum permissible length for user name in characters, truncated past this. */
constexpr auto NAME_MAX_LENGTH = 30;

/**
 * Launch the bot for a player, send the initial game information to the player, and update its name.
 * Safe to invoke from multiple threads on different players.
 *
 * @param player The player to communicate with.
 */
void Networking::initialize_player(Player &player) {
    Logging::log("Initializing player " + to_string(player.id) + " with command " + player.command);
    auto connection = connection_factory.new_connection(player.command);
    std::stringstream message_stream;
    Logging::log("Sending init message to player " + to_string(player.id));
    // Send the number of players and player ID
    message_stream << game.store.players.size()
                   << " " << player.id << std::endl;
    // Send each player's ID and factory location
    for (const auto &[player_id, other_player] : game.store.players) {
        message_stream << player_id
                       << " " << other_player.factory
                       << std::endl;
    }
    // Send the map
    message_stream << game.map;

    {
        std::lock_guard<std::mutex> guard(connections_mutex);
        connections[player] = std::move(connection);
    }

    try {
        connections[player]->send_string(message_stream.str());
        Logging::log("Init message sent to player " + to_string(player.id));
        // Receive a name from the player.
        player.name = connections[player]->get_string().substr(0, NAME_MAX_LENGTH);
        Logging::log("Init message received from player " + to_string(player.id) + ", name: " + player.name);
    }
    catch (const BotError& e) {
        player.log_error(e.what());
        const auto received_input = connections[player]->read_trailing_input();
        player.log_error("Last input received was:");
        player.log_error(received_input);
        throw;
    }
}

/**
 * Handle the networking for a single frame, obtaining commands from the player if there are any.
 * Safe to invoke from multiple threads on different players.
 *
 * @param player The player to communicate with.
 * @return The commands from the player.
 */
std::vector<std::unique_ptr<Command>> Networking::handle_frame(Player &player) {
    std::stringstream message_stream;
    // Send the turn number, then each player in the game.
    message_stream << game.turn_number << std::endl;
    for (const auto &[_, other_player] : game.store.players) {
        message_stream << other_player;
        // Output a list of entities.
        for (const auto &[entity_id, location] : other_player.entities) {
            const auto entity_iterator = game.store.entities.find(entity_id);
            message_stream << entity_id
                    << " " << location
                    << " " << entity_iterator->second.energy
                    << std::endl;
        }
        // Output a list of dropoffs.
        for (const auto &dropoff : other_player.dropoffs) {
            message_stream << dropoff << std::endl;
        }
    }
    // Send the changed cells.
    message_stream << game.store.changed_cells.size() << std::endl;
    for (const auto &location : game.store.changed_cells) {
        message_stream << location << " " << game.map.at(location).energy << std::endl;
    }

    std::vector<std::unique_ptr<Command>> commands;
    std::string received_input;
    try {
        connections[player]->send_string(message_stream.str());
        Logging::log("Turn info sent to player " + to_string(player.id), Logging::Level::Debug);
        // Get commands from the player.
        received_input = connections[player]->get_string();
        std::istringstream command_stream(received_input);
        std::unique_ptr<Command> command;
        while (command_stream >> command) {
            commands.push_back(std::move(command));
        }
        command_stream >> command;
        Logging::log("Received " + std::to_string(commands.size()) +
                     " commands from player " + to_string(player.id), Logging::Level::Debug);
    }
    catch (const BotError& e) {
        player.log_error(e.what());
        received_input += '\n';
        received_input += connections[player]->read_trailing_input();
        player.log_error("Last input received was:");
        player.log_error(received_input);
        throw;
    }

    return commands;
}

}
