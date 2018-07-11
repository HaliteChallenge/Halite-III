#include <future>
#include <sstream>

#include "BotError.hpp"
#include "BotCommandError.hpp"
#include "NetworkingError.hpp"
#include "HaliteImpl.hpp"

namespace hlt {

/** Communicate with bots to obtain commands for next step. */
void HaliteImpl::retrieve_commands() {
    std::unordered_map<Player::id_type, std::future<std::vector<Command>>> results;
    for (auto &[player_id, player] : game.players) {
        if (!player.alive) continue;

        results[player_id] = std::async(std::launch::async,
                                        [&game = this->game, &player = player] {
                                            return game.networking.handle_frame(player);
                                        });
    }

    std::ostringstream buf;
    buf << "Turn " << this->game.turn_number;

    for (auto &[player_id, result] : results) {
        if (!this->game.players[player_id].alive) continue;

        this->game.players[player_id].log_error_section(buf.str());
        try {
            commands[player_id] = result.get();
        }
        catch (const BotError& e) {
            // Already caught and logged in handle_frame, just kill the player
            this->game.kill_player(player_id);
        }
    }
}

/** Process the effects of commands. */
void HaliteImpl::process_commands() {
    for (const auto &[player_id, command_list] : commands) {
        if (!this->game.players[player_id].alive) continue;

        auto transaction = CommandTransaction(game.game_map, game.players[player_id]);
        transaction.set_callback([&frames = game.replay_struct.full_frames](auto event) {
            // Create new game event for replay file,
            // regardless of whether spawn creates a new entity or adds to an old entity
            // Ensure full frames has been initialized (ie don't do this before first turn)
            if (!frames.empty()) {
                frames.back().events.push_back(std::move(event));
            }
        });
        for (const auto &command : command_list) {
            command->act_on_map(transaction);
        }
        if (!transaction.commit()) {
            this->game.kill_player(player_id);
        }
    }
    // Add commands to replay struct for visualizer
    game.replay_struct.full_frames.back().moves = std::move(commands);
}

}
