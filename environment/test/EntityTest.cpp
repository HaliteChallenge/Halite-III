#include "Entity.hpp"

#include "nlohmann/json.hpp"

#include "catch.hpp"

using namespace hlt;

SCENARIO("Player Entity constructor initializes member variables", "[entity]") {
    GIVEN("An entity with owner and energy") {
        PlayerEntity entity(0, 10);
        REQUIRE(entity.owner_id == 0);
        REQUIRE(entity.energy == 10);
        WHEN("converted to json") {
            nlohmann::json json;
            to_json(json, entity);
            THEN("json has owner and energy of correct value") {
                REQUIRE(json.at("owner_id") == 0);
                REQUIRE(json.at("energy") == 10);
            }
        }
        WHEN("converted to bot serial format") {
            std::stringstream stream;
            stream << entity;
            THEN("output has owner and energy of correct values") {
                REQUIRE(stream.str() == "0 10\n");
            }
        }
    }
}
