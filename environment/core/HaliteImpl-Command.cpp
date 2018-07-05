#include <future>

#include "BotCommandError.hpp"
#include "HaliteImpl.hpp"

namespace hlt {

/** Communicate with bots to obtain commands for next step. */
void HaliteImpl::retrieve_commands() {
    std::unordered_map<Player::id_type, std::future<std::vector<Command>>> results;
    for (auto &[player_id, player] : game.players) {
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
    for (const auto &[player_id, command_list] : commands) {
        auto transaction = CommandTransaction(game.game_map, game.players[player_id]);
        for (const auto &command : command_list) {
            command->act_on_map(transaction);
        }
        if (!transaction.commit_moves()) {
            throw BotCommandError("Invalid move commands");
        }
        std::vector<GameEvent> spawns;
        if (!transaction.commit_spawn(spawns)) {
            throw BotCommandError("Invalid spawn commands");
        }
        for(auto &event : spawns) {
            // Create new game event for replay file, regardless of whether spawn creates a new entity or adds to an old entity
            // Ensure full frames has been initialized (ie don't do this before first turn)
            if (game.replay_struct.full_frames.size() > 0) {
                game.replay_struct.full_frames.back().events.emplace_back(std::move(event));
            }
        }
    }
    // Add commands to replay struct for visualizer
    game.replay_struct.full_frames.back().moves = commands;
}

}
