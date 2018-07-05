#ifndef ENTITY_H
#define ENTITY_H

#include "Constants.hpp"
#include "Player.hpp"

namespace hlt {

/**
 * Factory method for Entities to hide the constructor from clients.
 *
 * @tparam EntityType The class of the Entity.
 * Currently we only have the class entity, but use templating to permit expansion of Entity types
 * @tparam Args The types of the arguments to the Entity constructor.
 * @param args The arguments to the Entity constructor.
 * @return The newly constructed Entity.
 */
template<class EntityType, typename... Args>
std::shared_ptr<EntityType> make_entity(Args &&... args) {
    return std::make_shared<EntityType>(std::forward<Args>(args)...);
}

/** A player-affiliated entity placed on the Halite map. */
struct PlayerEntity {
    Player::id_type owner_id{}; /**< ID of the owner. */
    energy_type energy{};       /**< Energy of the entity. */

    /**
     * Convert an Entity to JSON format.
     * @param[out] json The output JSON.
     * @param entity The entity to convert.
     * @param include_owner If true, add the owner ID to the JSON.
     */
    friend void to_json(nlohmann::json &json, const PlayerEntity &entity, bool include_owner);

    /**
     * Convert an encoded Entity from JSON format.
     * @param json The JSON.
     * @param[out] entity The converted entity.
     */
    friend void from_json(const nlohmann::json &json, PlayerEntity &entity);

    /**
     * Write an Entity to bot serial format.
     * @param ostream The output stream.
     * @param entity The entity to write.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const PlayerEntity &entity);

    /** Default constructor is required by JSON deserialization. */
    PlayerEntity() = default;

    /**
     * Create Entity from owner ID, and energy.
     * @param owner_id The owner ID.
     * @param energy The energy of the entity
     */
    PlayerEntity(Player::id_type owner_id, energy_type energy) :
            owner_id(owner_id), energy(energy) {}
};

/** This redeclaration only serves to bind the default argument. */
void to_json(nlohmann::json &json, const PlayerEntity &entity, bool include_owner = true);

}

#endif // ENTITY_H

