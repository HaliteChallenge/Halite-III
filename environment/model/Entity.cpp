#include "Entity.hpp"

namespace hlt {

void to_json(nlohmann::json &json, const Entity &entity) {
    json = {{"entity_id", entity.entity_id},
            {"location",  entity.location},
            {"energy",    entity.energy}};
}

void from_json(const nlohmann::json &json, Entity &entity) {
    entity = {json.at("entity_id").get<Entity::EntityID>(),
              json.at("location").get<Location>(),
              json.at("energy").get<unsigned char>()};
}

std::ostream &operator<<(std::ostream &ostream, const Entity &entity) {
    return ostream << entity.entity_id << " " << entity.energy << std::endl;
}

}
