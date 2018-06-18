#include "HaliteImpl.hpp"

namespace hlt {

/**
 * Communicate with bots to obtain commands for next step.
 * @return Commands received from players.
 */
std::unordered_map<id_type, Command> HaliteImpl::retrieve_commands() {
    std::unordered_map<id_type, Command> commands;
    for (auto &player : game->players) {
        Command command;
        game->networking.handle_frame(player.second, command);
        commands[player.first] = std::move(command);
    }
    return commands;
}

/**
 * Process the effects of commands.
 * @param commands The commands to process.
 */
void HaliteImpl::process_commands(const std::unordered_map<id_type, Command> &commands) {
    for (const auto &player_command : commands) {
        auto &player = game->players[player_command.first];
        auto &command = player_command.second;
        command->act_on_map(game->game_map, player);
    }
}

/**
 * Process a spawn operation at a location, possibly merging onto an existing entity.
 * @param player The player owning the new entity.
 * @param location The location at which to spawn.
 */
void HaliteImpl::spawn_entity(Player &player, const Location& location) {
    const auto &constants = Constants::get();
    auto &entities = player.entities;
    auto entity_iterator = entities.find(location);
    if (entity_iterator != entities.end()) {
        // If there is already an entity, merge.
        entity_iterator->second->energy += constants.NEW_ENTITY_ENERGY;
    } else {
        // Otherwise, spawn.
        auto entity = make_entity<Entity>(player.player_id, constants.NEW_ENTITY_ENERGY);
        entities[location] = entity;
        game->game_map.at(location)->entities[player.player_id] = std::move(entity);
    }
}

/** Process all entity lifecycle events for this turn. */
void HaliteImpl::process_entities() {
    // Each factory may spawn a new entity
    const auto &constants = Constants::get();
    for (auto &player_pair : game->players) {
        auto &player = player_pair.second;
        if (player.energy >= constants.NEW_ENTITY_ENERGY_COST) {
            player.energy -= constants.NEW_ENTITY_ENERGY_COST;
            spawn_entity(player, player.factory_location);
        }
    }
    // Each entity loses some health, each entity with no remaining energy is removed
    for (auto &player_pair : game->players) {
        auto &entities = player_pair.second.entities;
        for (auto entity = entities.begin(); entity != entities.end();) {
            entity->second->energy -= constants.BASE_TURN_ENERGY_LOSS;
            if (entity->second->energy <= 0) {
                entities.erase(entity++);
            } else {
                entity++;
            }
        }
    }
}

}
