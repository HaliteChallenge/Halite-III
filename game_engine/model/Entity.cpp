#include "Entity.hpp"

/** A JSON key and value corresponding to a field. */
#define FIELD_TO_JSON(x) {#x, entity.x}

namespace hlt {

/**
 * Convert an Entity to JSON format.
 * @param[out] json The output JSON.
 * @param entity The entity to convert.
 */
void to_json(nlohmann::json &json, const Entity &entity) {
    json = {FIELD_TO_JSON(id),
            FIELD_TO_JSON(is_inspired),
            FIELD_TO_JSON(energy)};
}

/**
 * Write an Entity to bot serial format.
 * @param ostream The output stream.
 * @param entity The entity to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Entity &entity) {
    // Output the entity ID, then energy.
    return ostream << entity.id << " " << entity.energy;
}

}
