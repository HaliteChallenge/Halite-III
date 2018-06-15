#ifndef ENTITY_H
#define ENTITY_H

#include "Location.hpp"

namespace hlt {

/** An entity placed on the Halite map. */
struct Entity {
    friend class EntityFactory;

    /** Type of Entity IDs. */
    using id_type = long;

    id_type entity_id{};   /**< ID of the entity. */
    Location location{};    /**< Location of the entity. */
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
    bool operator==(const Entity &other) const { return entity_id == other.entity_id; }

    /** Order two Entity instances by ID. */
    bool operator<(const Entity &other) const { return entity_id < other.entity_id; }

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
    Entity(Entity::id_type entity_id, const Location &location, energy_type energy) :
            entity_id(entity_id), location(location), energy(energy) {}
};

/** Factory producing Entity instances. */
class EntityFactory {
    /** The next Entity to allocate, starting at zero. */
    Entity::id_type next_entity{};

public:
    /**
     * Make a new entity.
     * @param location The location of the entity.
     * @param energy The energy of the entity.
     * @return The new entity.
     */
    Entity new_entity(const Location &location, energy_type energy) {
        return {next_entity++, location, energy};
    };

    EntityFactory() = default;
};

}

namespace std {
template<>
struct hash<hlt::Entity> {
    size_t operator()(const hlt::Entity &entity) const {
        return (size_t) entity.entity_id;
    }
};
}

#endif // ENTITY_H

