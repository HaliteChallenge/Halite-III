#include "nlohmann/json.hpp"

#include "catch.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#define protected public
#define private public
#pragma clang diagnostic pop

#include "Player.hpp"

using namespace hlt;

SCENARIO("Player constructor initializes member variables", "[player]") {
    GIVEN("A new player") {
    }
}

SCENARIO("Player operators are semantically correct", "[player_operators]") {
    GIVEN("Some players") {
        WHEN("players are compared for equality") {
            THEN("equal players are equal") {
            }
            THEN("unequal players are unequal") {
            }
        }
        WHEN("players are compared by order") {
            THEN("the ordering relation is correct") {
            }
        }
        WHEN("equal players are hashed") {
            THEN("the hashes are equal") {
            }
        }
    }
}

SCENARIO("Players are encoded to JSON and bot serial format correctly", "[player_serial]") {
    GIVEN("A player") {
        WHEN("player is serialized to JSON") {
            THEN("contents of JSON are correct") {
            }
        }
        WHEN("player is serialized to bot serial format") {
            THEN("output is correct") {
            }
        }
    }
}

SCENARIO("Players and entities interact correctly", "[player_entities]") {
    WHEN("entity added") {
        THEN("entity is stored by player, has correct energy") {
        }
    }
}

SCENARIO("Player factory initializes player IDs", "[player_factory]") {
    GIVEN("A new player factory") {
        WHEN("a player is created") {
            THEN("the new player is initialized correctly") {
            }
        }
        WHEN("another player is created") {
            THEN("the second player is initialized correctly") {
            }
        }
    }
}
