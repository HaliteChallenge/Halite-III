#include "Entity.hpp"

namespace hlt {

void to_json(nlohmann::json &json, const Entity &entity) {
    json = {{"entity_id", entity.entity_id},
            {"location",  entity.location},
            {"energy",    entity.energy}};
}

void from_json(const nlohmann::json &json, Entity &entity) {
    entity = {json.at("entity_id").get<decltype(entity.entity_id)>(),
              json.at("location").get<decltype(entity.location)>(),
              json.at("energy").get<decltype(entity.energy)>()};
}

std::ostream &operator<<(std::ostream &ostream, const Entity &entity) {
    return ostream << entity.entity_id << " " << entity.energy << std::endl;
}

}
