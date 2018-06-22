#include "HaliteImpl.hpp"

namespace hlt {

/** Communicate with bots to obtain commands for next step. */
void HaliteImpl::retrieve_commands() {
    for (auto &[player_id, player] : game->players) {
        Command command;
        game->networking.handle_frame(player, command);
        commands[player_id] = std::move(command);
    }
}

/** Process the effects of commands. */
void HaliteImpl::process_commands() {
    for (const auto &[player_id, command] : commands) {
        command->act_on_map(game->game_map, game->players[player_id]);
    }
}

}
