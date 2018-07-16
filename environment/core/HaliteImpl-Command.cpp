#include <future>

#include "BotCommandError.hpp"
#include "HaliteImpl.hpp"

namespace hlt {

/** Communicate with bots to obtain commands for next step. */
void HaliteImpl::retrieve_commands() {
    std::unordered_map<Player::id_type, std::future<std::vector<std::unique_ptr<Command>>>> results;
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
    CommandTransaction transaction{game.game_map};
    for (const auto &[player_id, command_list] : commands) {
        auto &player = game.players.find(player_id)->second;
        for (const auto &command : command_list) {
            command->add_to_transaction(player, transaction);
        }
    }
    if (transaction.check()) {
        transaction.commit();
    } else {
        // TODO: handle error
    }
}

}
