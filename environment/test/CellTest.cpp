#include "Cell.hpp"

#include "catch.hpp"

using namespace hlt;

SCENARIO("Cell is initialized correctly", "[cell]") {
    GIVEN("An empty cell") {
        Cell cell;
        REQUIRE(cell.energy == 0);
        REQUIRE(cell.entity == Entity::None);
        REQUIRE(cell.owner == Player::None);
        WHEN("converted to json") {
            nlohmann::json json;
            to_json(json, cell);
            THEN("json has correct values") {
                REQUIRE(json == nlohmann::json{{"energy", 0}});
            }
        }
        WHEN("converted to bot serial format") {
            std::stringstream stream;
            stream << cell;
            THEN("output has correct value") {
                REQUIRE(stream.str() == "0");
            }
        }
    }
    GIVEN("A cell with energy, entity, and owner") {
        Cell cell{1, Entity::id_type(2), Player::id_type(3)};
        REQUIRE(cell.energy == 1);
        REQUIRE(cell.entity == Entity::id_type(2));
        REQUIRE(cell.owner == Player::id_type(3));
        WHEN("converted to json") {
            nlohmann::json json;
            to_json(json, cell);
            THEN("json has correct values") {
                REQUIRE(json == nlohmann::json{{"energy", 1}});
            }
        }
        WHEN("converted to bot serial format") {
            std::stringstream stream;
            stream << cell;
            THEN("output has correct value") {
                REQUIRE(stream.str() == "1");
            }
        }
    }
}
