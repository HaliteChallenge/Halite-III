#ifndef ENTITY_H
#define ENTITY_H

#include "Constants.hpp"

namespace hlt {

/**
 * Factory method for Entities to hide the constructor from clients.
 *
 * @tparam EntityType The class of the Entity. Currently we only have the class entity, but use templeting to permit expansion
 *  of Entity types
 * @tparam Args The types of the arguments to the Entity constructor.
 * @param args The arguments to the Entity constructor.
 * @return The newly constructed Entity.
 */
template<class EntityType, typename... Args>
std::shared_ptr<EntityType> make_entity(Args &&... args) {
    return std::make_shared<EntityType>(std::forward<Args>(args)...);
}

/** An entity placed on the Halite map. */
struct Entity {
    friend class EntityFactory;

    id_type owner_id{};   /**< ID of the entity. */
    energy_type energy{}; /**< Energy of the entity. */

    /**
     * Convert an Entity to JSON format.
     * @param[out] json The output JSON.
     * @param entity The entity to convert.
     */
    friend void to_json(nlohmann::json &json, const Entity &entity);

    /**
     * Convert an encoded Entity from JSON format.
     * @param json The JSON.
     * @param[out] entity The converted entity.
     */
    friend void from_json(const nlohmann::json &json, Entity &entity);

    /** Test two Entity instances for equality. */
    // TODO
    // bool operator==(const Entity &other) const { return entity_id == other.entity_id; }

    /** Order two Entity instances by energy */
    bool operator<(const Entity &other) const { return energy < other.energy; }

    /**
     * Write an Entity to bot serial format.
     * @param ostream The output stream.
     * @param entity The entity to write.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const Entity &entity);

    /** Default constructor is required by JSON deserialization. */
    Entity() = default;

private:
    /**
     * Create Entity from ID, location, and energy.
     * @param entity_id The entity ID.
     * @param location The location.
     * @param energy The energy.
     */
    Entity(id_type owner_id, energy_type energy) :
            owner_id(owner_id), energy(energy) {}
};

/** Factory producing Entity instances. */
class EntityFactory {
public:
    /**
     * Make a new entity.
     * @param location The location of the entity.
     * @param energy The energy of the entity.
     * @return The new entity.
     */
    Entity new_entity(id_type owner_id, energy_type energy) {
        return {owner_id, energy};
    };

    EntityFactory() = default;
};

}

// TODO: determine appropriate hash
//namespace std {
//template<>
//struct hash<hlt::Entity> {
//    size_t operator()(const hlt::Entity &entity) const {
//        return (size_t) entity.entity_id;
//    }
//};
//}

#endif // ENTITY_H

