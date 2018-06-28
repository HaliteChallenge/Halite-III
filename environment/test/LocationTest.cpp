#include "Location.hpp"
#include "nlohmann/json.hpp"

#include "catch.hpp"

using namespace hlt;

TEST_CASE("Direction can be converted to expected json", "[direction_json]") {
    nlohmann::json json;
    SECTION("North converted correctly") {
        to_json(json, hlt::Direction::North);
        REQUIRE(json == nlohmann::json("n"));
    }
    SECTION("South converted correctly") {
        to_json(json, hlt::Direction::South);
        REQUIRE(json == nlohmann::json("s"));
    }
    SECTION("East converted correctly") {
        to_json(json, hlt::Direction::East);
        REQUIRE(json == nlohmann::json("e"));
    }
    SECTION("West converted correctly") {
        to_json(json, hlt::Direction::West);
        REQUIRE(json == nlohmann::json("w"));
    }
}

TEST_CASE("Direction can be converted from bot serial format", "[direction_bot_serial]") {
    Direction direction;
    SECTION("North converted correctly") {
        std::istringstream("n") >> direction;
        REQUIRE(direction == Direction::North);
    }
    SECTION("South converted correctly") {
        std::istringstream("s") >> direction;
        REQUIRE(direction == Direction::South);
    }
    SECTION("East converted correctly") {
        std::istringstream("e") >> direction;
        REQUIRE(direction == Direction::East);
    }
    SECTION("West converted correctly") {
        std::istringstream("w") >> direction;
        REQUIRE(direction == Direction::West);
    }
    SECTION("Invalid directions fail to convert") {
        REQUIRE_THROWS(std::istringstream("fail") >> direction);
    }
}
