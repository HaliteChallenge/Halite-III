#include "Auction.hpp"

namespace hlt {

/**
 * Determine if any player successfully purchased the factory. Fill eligible players with number of players offering
 * the winning price. Also updates the minimum bid to be the highest offered price. Clears the offers after determining
 * the winner, if any.
 *
 * Assumes all offers are valid (ie greater than minimum price, and player is eligible to buy the factory).
 * A tie is indicated by the size of eligible players being greater than 1.
 * return Price factory was bought for, or price of tied bids
 */
energy_type Auction::determine_buyer(){
    energy_type highest_price = std::numeric_limits<energy_type>::min();
    for (const auto [player_id, price] : player_buy_offers) {
        if (price > highest_price) {
            eligible_players.clear();
            eligible_players.insert(player_id);
        } else if (price == highest_price) {
            eligible_players.insert(player_id);
        }
    }
    player_buy_offers.clear();
    // TODO: determine appropriate action if there are no buyers.
    return highest_price;
}

}