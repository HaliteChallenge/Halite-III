#ifndef AUCTION_HPP
#define AUCTION_HPP

#include <unordered_set>

#include "Player.hpp"
namespace hlt {

/**
 * Information pertaining to sale of factory
 */
class Auction {
public:
    energy_type minimum_price;                                          /**< Minimum price needed to purchase factory. */
    Player::id_type previous_owner;                                     /**< Player putting factory up for sale. */
    energy_type previous_purchase_price;                                /**< Price player putting factory up for sale paid for it. */
    std::unordered_set<Player::id_type> eligible_players;               /**< Players eligible to purchase (in case of ties). Empty implies all players may purchase. */

    std::unordered_map<Player::id_type, energy_type> player_buy_offers; /**< Amount a player offered to buy the factory. */

    /**
     * Determine if any player successfully purchased the factory. Fill eligible players with number of players offering
     * the winning price. Also updates the minimum bid to be the highest offered price. Clears the offers after determining
     * the winner, if any.
     *
     * Assumes all offers are valid (ie greater than minimum price, and player is eligible to buy the factoy).
     * A tie is indicated by the size of eligible players being greater than 1.
     * return Price factory was bought for, or price of tied bids
     */
    energy_type determine_buyer();

};
}
#endif // AUCTION_HPP