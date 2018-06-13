#include "Entity.hpp"

#include "nlohmann/json.hpp"

using namespace std::literals::string_literals;

/** The JSON key for entity ID. */
constexpr auto JSON_ENTITY_ID_KEY = "entity_id";
/** The JSON key for location. */
constexpr auto JSON_LOCATION_KEY = "location";
/** The JSON key for energy. */
constexpr auto JSON_ENERGY_KEY = "energy";

namespace hlt {

/**
 * Convert an Entity to JSON format.
 * @param[out] json The output JSON.
 * @param entity The entity to convert.
 */
void to_json(nlohmann::json &json, const Entity &entity) {
    json = {{JSON_ENTITY_ID_KEY, entity.entity_id},
            {JSON_LOCATION_KEY,  entity.location},
            {JSON_ENERGY_KEY,    entity.energy}};
}

/**
 * Convert an encoded Entity from JSON format.
 * @param json The JSON.
 * @param[out] entity The converted entity.
 */
void from_json(const nlohmann::json &json, Entity &entity) {
    entity = {json.at(JSON_ENTITY_ID_KEY), json.at(JSON_LOCATION_KEY), json.at(JSON_ENERGY_KEY)};
}

/**
 * Write an Entity to bot serial format.
 * @param ostream The output stream.
 * @param entity The entity to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Entity &entity) {
    // Output the entity ID then its energy.
    return ostream << entity.entity_id << " " << entity.energy << std::endl;
}

}
