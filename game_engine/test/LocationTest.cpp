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

SCENARIO("Location is initialized correctly", "[location]") {
    GIVEN("A location from coordinates") {
        Location location{1, 2};
        REQUIRE(location.x == 1);
        REQUIRE(location.y == 2);
        WHEN("location is converted to bot serial format") {
            std::stringstream stream;
            stream << location;
            THEN("output is correct") {
                REQUIRE(stream.str() == "1 2");
            }
        }
        AND_WHEN("location is converted to json") {
            nlohmann::json json;
            to_json(json, location);
            THEN("output is correct") {
                nlohmann::json expected{{"x", location.x}, {"y", location.y}};
                REQUIRE(json == expected);
            }
        }
    }
}

SCENARIO("Location comparison and hashing are correct", "[location_equality]") {
    GIVEN("Some locations") {
        Location location1{1, 2};
        Location location2{1, 2};
        Location location3{2, 3};
        WHEN("equal locations are compared") {
            THEN("they are equal") {
                REQUIRE(location1 == location2);
                REQUIRE(location2 == location1);
            }
            AND_THEN("their hashes are equal") {
                REQUIRE(std::hash<Location>()(location1) == std::hash<Location>()(location2));
            }
        }
        AND_WHEN("unequal locations are compared") {
            THEN("they are not equal") {
                REQUIRE(!(location1 == location3));
                REQUIRE(!(location2 == location3));
            }
        }
    }
}
