#include "Location.hpp"
#include "nlohmann/json.hpp"

#include "catch.hpp"

TEST_CASE("Direction can be converted to expected json", "[direction_json]") {
    SECTION( " North converted correctly") {
        auto north = hlt::Direction::North;
        nlohmann::json json;
        nlohmann::to_json(json, north);

        REQUIRE(json == nlohmann::json("n"));
    }
    SECTION( " South converted correctly") {
        auto south = hlt::Direction::South;
        nlohmann::json json;
        nlohmann::to_json(json, south);
        REQUIRE(json == nlohmann::json("s"));
    }
    SECTION( " East converted correctly") {
        auto east = hlt::Direction::East;
        nlohmann::json json;
        nlohmann::to_json(json, east);
        REQUIRE(json == nlohmann::json("e"));
    }
    SECTION( " West converted correctly") {
        auto west = hlt::Direction::West;
        nlohmann::json json;
        nlohmann::to_json(json, west);
        REQUIRE(json == nlohmann::json("w"));
    }
}