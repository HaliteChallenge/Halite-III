#include "Cell.hpp"

#include "catch.hpp"

using namespace hlt;

SCENARIO("Cell constructor initializes member variables", "[cell]") {
    GIVEN("A normal cell with production") {
        const auto BASE_PRODUCTION = 10;
        hlt::Cell normal_cell{};
        normal_cell.energy = BASE_PRODUCTION;
        REQUIRE(normal_cell.energy == BASE_PRODUCTION);
        WHEN("converted to json") {
            nlohmann::json json;
            to_json(json, normal_cell);
            THEN("json has type and production of correct values") {
                REQUIRE(json.at("energy") == BASE_PRODUCTION);
            }
        }
        WHEN("converted to bot serial format") {
            std::stringstream stream;
            stream << normal_cell;
            THEN("output correct production value") {
                REQUIRE(stream.str() == "10\n");
            }
        }
    }
}
