#include "HaliteImpl.hpp"
#include "Constants.hpp"

namespace hlt {

/**
 * Determine minimum purchase price for factories at start of game
 * @return minimum purchase price
 */
energy_type HaliteImpl::initial_minimum_price() {
    const auto initial_player_energy = Constants::get().INITIAL_ENERGY;
    return initial_player_energy / game.game_map.factory_count;

}

/**
 * Execute initial factory division, including dealing with ties. This function will deal with processing commands from
 * players and sending them info after each tie break
 * Will assign players' their initial factory.
 */
void HaliteImpl::initial_factory_assignment() {
    //TODO
}
}
