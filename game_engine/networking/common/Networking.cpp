#include <sstream>
#include <algorithm>

#include "Command.hpp"
#include "Halite.hpp"
#include "Replay.hpp"
#include "Logging.hpp"
#include "BotError.hpp"

namespace net {

using namespace hlt;

/** Maximum permissible length for user name in characters, truncated past this. */
constexpr auto NAME_MAX_LENGTH = 30;

/**
 * Handle a player communication error.
 * @param player The player ID.
 * @param received_input The current received input from the player.
 */
void Networking::handle_player_error(Player::id_type player, std::string received_input) {
    auto &connection = connections.get(player);
    try {
        received_input += connection->read_trailing_input();
    } catch (...) {
        // Ignore any exceptions from the read attempt.
    }
    if (!received_input.empty()) {
        game.logs.log(player, "Last input received was:");
        game.logs.log(player, received_input);

        // Log line by line
        Logging::log_lines("Last input received was:\n" + received_input, Logging::Level::Error, player);
    }
    std::string errors;
    try {
        errors = connection->get_errors();
    } catch (...) {
        // Ignore any exceptions from the read attempt.
    }
    if (!errors.empty()) {
        game.logs.log(player, "Bot error output was:");
        game.logs.log(player, errors);

        // Log line by line
        Logging::log_lines("Player's error output was:\n" + errors, Logging::Level::Error, player);
    }
}

/**
 * Launch the bot for a player and register their connection.
 *
 * @param player The player.
 */
void Networking::connect_player(hlt::Player &player) {
    connections.add(player.id, connection_factory.new_connection(player.command));
}

/**
 * Launch the bot for a player, send the initial game information to the player, and update its name.
 * Safe to invoke from multiple threads on different players.
 *
 * @param player The player to communicate with.
 */
void Networking::initialize_player(Player &player) {
    std::stringstream message_stream;
    Logging::log("Sending init message", Logging::Level::Debug, player.id);
    // Send the game constants
    nlohmann::json constants = Constants::get();
    constants["game_seed"] = game.replay.map_generator_seed;
    constants["map_width"] = game.map.width;
    constants["map_height"] = game.map.height;
    message_stream << constants.dump() << std::endl;

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

    try {
        connections.get(player.id)->send_string(message_stream.str());
        Logging::log("Init message sent", Logging::Level::Debug, player.id);
        // Receive a name from the player.
        static constexpr auto INIT_TIMEOUT = std::chrono::seconds(30);
        auto name = connections.get(player.id)->get_string(INIT_TIMEOUT).substr(0, NAME_MAX_LENGTH);
        // On Windows, we get the \r character in names.
        name.erase(std::remove(name.begin(), name.end(), '\r'), name.end());
        player.name = name;
        Logging::log("Init message received, name: " + player.name,
                     Logging::Level::Debug, player.id);
    } catch (const BotError &e) {
        Logging::log("Failed to initialize", Logging::Level::Error, player.id);
        game.logs.log(player.id, e.what(), PlayerLog::Level::Error);
        handle_player_error(player.id);
        player.name = "(failed to initialize)";
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
        connections.get(player.id)->send_string(message_stream.str());
        Logging::log("Turn info sent", Logging::Level::Debug, player.id);
        // Get commands from the player.
        received_input = connections.get(player.id)->get_string();
        std::istringstream command_stream(received_input);
        std::unique_ptr<Command> command;
        while (command_stream >> command) {
            commands.push_back(std::move(command));
        }
        command_stream >> command;
        Logging::log([number = commands.size()]() {
            return "Received " + std::to_string(number) + " commands";
        }, Logging::Level::Debug, player.id);
    } catch (const BotError &e) {
        Logging::log("Communication failed", Logging::Level::Error, player.id);
        game.logs.log(player.id, e.what(), PlayerLog::Level::Error);
        received_input += '\n';
        handle_player_error(player.id, received_input);
        throw;
    }

    return commands;
}

/**
 * Kill a player connection.
 *
 * @param player The player whose connection to end.
 */
void Networking::kill_player(const hlt::Player &player) {
    const auto errors = connections.get(player.id)->get_errors();
    if (!errors.empty()) {
        game.logs.log(player.id, "Bot error output was:");
        game.logs.log(player.id, errors);
    }
    connections.remove(player.id);
}

}
