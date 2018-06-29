#include "HaliteImpl.hpp"

namespace hlt {

/**
 * Determine whether the game has ended.
 *
 * @return True if the game has ended.
 */
bool HaliteImpl::game_ended() const {
    long num_alive_players = 0;
    for (auto &&[_, player] : game.players) {
        // TODO: implement edge case of last player being unable to produce
        if (!player.entities.empty()) {
            num_alive_players++;
        }
        if (num_alive_players > 1) {
            return false;
        }
    }
    return true;
}

/**
 * Construct HaliteImpl from game interface.
 *
 * @param game The game interface.
 */
HaliteImpl::HaliteImpl(Halite &game) : game(game), ownership_grid(game.game_map.height, game.game_map.width) {}

}
