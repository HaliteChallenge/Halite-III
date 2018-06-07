#ifndef ENTITY_H
#define ENTITY_H

#include "Location.hpp"

namespace hlt {

/** An entity placed on the Halite map. */
struct Entity {
    friend class EntityFactory;

    using EntityID = int;

    constexpr static unsigned char max_energy = 255;

    EntityID entity_id{}; /**< ID of the entity. */
    Location location; /**< Location of the entity. */
    unsigned char energy{}; /**< Energy of the entity, from 1 to 255. */

    friend void to_json(nlohmann::json &json, const Entity &entity);

    friend void from_json(const nlohmann::json &json, Entity &entity);

    friend std::ostream &operator<<(std::ostream &os, const Entity &entity);

    Entity() = default;

private:
    Entity(EntityID entity_id, const Location &location, unsigned char energy = max_energy);
};

bool operator==(const Entity &e1, const Entity &e2);

bool operator<(const Entity &e1, const Entity &e2);

class EntityFactory {
    Entity::EntityID next_entity; /**< The next entity to allocate. */

public:
    /** Get a new player. */
    Entity new_entity(const Location &location);

    explicit EntityFactory(Entity::EntityID next_entity = 0);
};

}

namespace std {
template<>
struct hash<hlt::Entity> {
    size_t operator()(const hlt::Entity &e) const {
        return (size_t) e.entity_id;
    }
};
}

#endif // ENTITY_H

