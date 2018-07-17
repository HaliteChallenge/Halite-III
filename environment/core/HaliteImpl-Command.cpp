#include <future>

#include "BotCommandError.hpp"
#include "HaliteImpl.hpp"

namespace hlt {

/** Communicate with bots to obtain commands for next step. */
void HaliteImpl::retrieve_commands() {
    std::unordered_map<Player::id_type, std::future<std::vector<std::unique_ptr<Command>>>> results{};
    for (auto &[player_id, player] : game.store.players) {
        results[player_id] = std::async(std::launch::async,
                                        [&game = this->game, &player = player] {
                                            return game.networking.handle_frame(player);
                                        });
    }
    for (auto &[player_id, result] : results) {
        commands[player_id] = result.get();
    }
}

/** Process the effects of commands. */
void HaliteImpl::process_commands() {
    // Process valid player commands, removing players if they submit invalid ones.
    while (!commands.empty()) {
        CommandTransaction transaction{game.store, game.map};
        transaction.set_callback([&frames = game.replay.full_frames](auto event) {
            // Create new game event for replay file. Ensure turn has been initialized before adding a game event
            if (frames.size() > 0) {
                frames.back().events.push_back(std::move(event));
            }
        });
        for (const auto &[player_id, command_list] : commands) {
            auto &player = game.store.players.find(player_id)->second;
            for (const auto &command : command_list) {
                command->add_to_transaction(player, transaction);
            }
        }
        if (transaction.check()) {
            // All commands are successful.
            transaction.commit();
            // add player commands to replay and note players still alive
            game.replay.full_frames.back().moves = std::move(commands);
            for (auto &player_statistics : game.replay.game_statistics.player_statistics) {
                if (commands.find(player_statistics.player_id) != commands.end()) player_statistics.last_turn_alive = game.turn_number;
            }

            break;
        } else {
            // Remove this player and their commands, and try again.
            kill_player(transaction.offender());
        }
    }
}

}
