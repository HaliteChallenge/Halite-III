#include "IndependentEntity.hpp"

#define FIELD_TO_JSON(x) {#x, entity.x};

/**
 * Merges given entity with this entity by summing the total number of entities the merged entity represents
 * param other_entity The entity to merge with this one.
 */
void IndependentEntity::merge_entities(const Entity &other_entity) {
    this->entities_represented += other_entity.entities_represented;
}

/**
* Convert an Entity to JSON format.
* @param[out] json The output JSON.
* @param entity The entity to convert.
*/
void to_json(nlohmann::json &json, const IndependentEntity &entity) {
    json = {FIELD_TO_JSON(entities_represented)};
}

/**
 * Write an IndependentEntity to bot serial format.
 * @param ostream The output stream.
 * @param entity The entity to write.
 * @return The output stream.
 */
 std::ostream &operator<<(std::ostream &ostream, const IndependentEntity &entity) {
    return ostream <<entity.entities_represented << std::endl;
 }