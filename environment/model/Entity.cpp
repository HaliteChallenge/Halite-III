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

std::ostream &operator<<(std::ostream &os, const Entity &entity) {
    return os << entity.entity_id << " " << entity.energy << std::endl;
}

bool operator==(const Entity &e1, const Entity &e2) {
    return e1.entity_id == e2.entity_id;
}

bool operator<(const Entity &e1, const Entity &e2) {
    return e1.entity_id < e2.entity_id;
}

Entity::Entity(Entity::EntityID entity_id, const Location &location, unsigned char energy) :
        entity_id(entity_id), location(location), energy(energy) {}

Entity EntityFactory::new_entity(const Location &location) {
    return {next_entity++, location};
}

EntityFactory::EntityFactory(Entity::EntityID next_entity) : next_entity(next_entity) {}

}
