#include "HaliteImpl.hpp"

namespace hlt {

/** Process all entity lifecycle events for this turn. */
void HaliteImpl::process_entities() {
    const auto &constants = Constants::get();
    // Each entity loses some health, each entity with no remaining energy is removed
    for (auto &[player_id, player] : game.players) {
        auto &entities = player.entities;
        for (auto entity_iterator = entities.begin(); entity_iterator != entities.end();) {
            auto [location, entity] = *entity_iterator;
            entity->energy -= constants.BASE_TURN_ENERGY_LOSS;
            if (entity->energy <= 0) {
                game.game_map.at(location)->remove_entity(player);
                // Create a new death event for replay file
                if (game.replay_struct.full_frames.size() > 0) {
                    GameEvent death_event = std::make_unique<DeathEvent>(location, entity->owner_id);
                    game.replay_struct.full_frames.back().events.push_back(std::move(death_event));
                }
                entities.erase(entity_iterator++);
            } else {
                entity_iterator++;
            }
        }
    }
}

}
