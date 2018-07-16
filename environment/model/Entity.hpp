#ifndef ENTITY_H
#define ENTITY_H

#include "Constants.hpp"
#include "Enumerated.hpp"

namespace hlt {

/** A player-affiliated entity placed on the Halite map. */
struct Entity : public virtual Enumerated<Entity> {
    friend class Factory<Entity>;

    /** Energy of the entity. */
    energy_type energy;

    /**
     * Convert an Entity to JSON format.
     * @param[out] json The output JSON.
     * @param entity The entity to convert.
     */
    friend void to_json(nlohmann::json &json, const Entity &entity);

    /**
     * Write an Entity to bot serial format.
     * @param ostream The output stream.
     * @param entity The entity to write.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const Entity &entity);

private:
    /**
     * Create Entity from ID and energy.
     * @param id The entity ID.
     * @param energy The energy.
     */
    Entity(id_type id, energy_type energy) : Enumerated(id), energy(energy) {}
};

}

#endif // ENTITY_H

