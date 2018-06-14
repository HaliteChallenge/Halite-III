#include "HaliteImpl.hpp"

namespace hlt {

/** Communicate with bots to obtain moves for next step. */
void HaliteImpl::retrieve_moves() {
    for (auto &player : game->players) {
        Command command;
        game->networking.handle_frame(player, command);
    }
}

/** Process all commands for this turn. */
void HaliteImpl::process_commands() {
    // TODO: implement
}

/** Process all entity lifecycle events for this turn. */
void HaliteImpl::process_entities() {
    // TODO: implement
}

}
