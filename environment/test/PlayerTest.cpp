#include <sstream>

#include "nlohmann/json.hpp"

#include "catch.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define protected public
#define private public
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "Player.hpp"

using namespace hlt;

SCENARIO("Players are initialized correctly", "[player]") {
    GIVEN("A new player") {
        Player player(Player::id_type(0), Location(0, 0), "bot");
        REQUIRE(player.id == Player::id_type(0));
        REQUIRE(player.factory == Location(0, 0));
        REQUIRE(player.command == "bot");
        REQUIRE(player.dropoffs.empty());
        REQUIRE(player.energy == 0);
        Entity::id_type entity1{0};
        Entity::id_type entity2{1};
        THEN("it should have no entities") {
            REQUIRE(player.entities.empty());
            REQUIRE(!player.has_entity(entity1));
            REQUIRE(!player.has_entity(entity2));
        }
        Location location1{1, 1};
        WHEN("an entity is added") {
            player.add_entity(entity1, location1);
            THEN("it should have the entity") {
                REQUIRE(player.has_entity(entity1));
                REQUIRE(!player.has_entity(entity2));
                REQUIRE(player.entities.size() == 1);
            }
            AND_THEN("it should have the entity location") {
                REQUIRE(player.get_entity_location(entity1) == location1);
            }
            Location location2{2, 2};
            AND_WHEN("another entity is added") {
                player.add_entity(entity2, location2);
                THEN("it should have both entities") {
                    REQUIRE(player.has_entity(entity1));
                    REQUIRE(player.has_entity(entity2));
                    REQUIRE(player.entities.size() == 2);
                }
                AND_THEN("it should have both entity locations") {
                    REQUIRE(player.get_entity_location(entity1) == location1);
                    REQUIRE(player.get_entity_location(entity2) == location2);
                }
            }
        }
    }
}

SCENARIO("Players are encoded to JSON and bot serial format correctly", "[player_serial]") {
    GIVEN("An empty player") {
        Player player(Player::id_type(0), Location(0, 0), "bot");
        WHEN("player is serialized to JSON") {
            nlohmann::json json;
            to_json(json, player);
            THEN("contents of JSON are correct") {
                REQUIRE(json == nlohmann::json{{"player_id", 0},
                                               {"name", ""},
                                               {"energy", 0},
                                               {"factory_location", Location(0, 0)},
                                               {"entities", nlohmann::json::array()}});
            }
        }
        WHEN("player is serialized to bot serial format") {
            std::stringstream stream;
            stream << player;
            THEN("output is correct") {
                REQUIRE(stream.str() == "0 0 0 0\n");
            }
        }
    }
    GIVEN("A player with energy, name, entities and dropoffs") {
        Player player(Player::id_type(1), Location(2, 3), "bot");
        player.name = "name";
        player.energy = 4;
        player.add_entity(Entity::id_type(5), Location(6, 7));
        player.dropoffs.emplace_back(Dropoff::id_type(7), Location(8, 9));
        WHEN("player is serialized to JSON") {
            nlohmann::json json;
            to_json(json, player);
            THEN("contents of JSON are correct") {
                REQUIRE(json == nlohmann::json{{"player_id", 1},
                                               {"name", "name"},
                                               {"energy", 4},
                                               {"factory_location", Location(2, 3)},
                                               {"entities", {{5, Location(6, 7)}}}});
            }
        }
        WHEN("player is serialized to bot serial format") {
            std::stringstream stream;
            stream << player;
            THEN("output is correct") {
                REQUIRE(stream.str() == "1 1 1 4\n");
            }
        }
    }
}
