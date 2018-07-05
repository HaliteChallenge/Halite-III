#ifndef INDEPENDENTENTITY_H
#define INDEPENDENTENTITY_H

#include "nlohmann/json_fwd.hpp"

/** A non-player-owned entity placed on the Halite map. Used for herding features of game play. */
class IndependentEntity {
public:
    long entities_represented = 1; /**< The number of entities that have merged together to form this one */

    /**
     * Merges given entity with this entity by summing the total number of entities the merged entity represents
     * param other_entity The entity to merge with this one.
     */
    void merge_entities(const IndependentEntity &other_entity);

    /**
    * Convert an Entity to JSON format.
    * @param[out] json The output JSON.
    * @param entity The entity to convert.
    */
    friend void to_json(nlohmann::json &json, const IndependentEntity &entity);

    /**
     * Write an Entity to bot serial format.
     * @param ostream The output stream.
     * @param entity The entity to write.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const IndependentEntity &entity);

    /**
     * Default constructor used to create new independent entities
     */
     IndependentEntity() = default;

};

#endif //INDEPENDENTENTITY_H
