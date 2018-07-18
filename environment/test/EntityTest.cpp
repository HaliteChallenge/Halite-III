#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#define protected public
#define private public
#pragma clang diagnostic pop

#include "Entity.hpp"

#include "catch.hpp"

using namespace hlt;

SCENARIO("Entity constructor initializes member variables", "[entity]") {
    GIVEN("An entity with energy") {
        Entity::id_type id = Entity::id_type(1);
        id_type<Player> owner = id_type<Player>(0);
        Entity entity(id, owner, 10);
        REQUIRE(entity.id == id);
        REQUIRE(entity.owner == owner);
        REQUIRE(entity.energy == 10);
        WHEN("converted to json") {
            nlohmann::json json;
            to_json(json, entity);
            THEN("json has id and energy of correct value") {
                REQUIRE(json.at("id") == 1);
                REQUIRE(json.at("energy") == 10);
            }
        }
        WHEN("converted to bot serial format") {
            std::stringstream stream;
            stream << entity;
            THEN("output has id and energy of correct values") {
                REQUIRE(stream.str() == "1 10");
            }
        }
    }
}
