#include "nlohmann/json.hpp"

#include "catch.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#define protected public
#define private public
#pragma clang diagnostic pop

#include "Entity.hpp"

using namespace hlt;

SCENARIO("Player constructor initializes member variables", "[player]") {
    GIVEN("An empty player") {
        Player player;
        REQUIRE(player.player_id == 0);
    }

    GIVEN("A new player") {
        Player player(1, "player", "./bot");
        REQUIRE(player.player_id == 1);
        REQUIRE(player.name == "player");
        REQUIRE(player.command == "./bot");
        REQUIRE(player.energy == 0);
        REQUIRE(player.factories.empty());
        REQUIRE(player.entities.empty());
    }

    GIVEN("A reconstructed player") {
        Player player(1, "player", 2, {{3, 4}}, {});
        REQUIRE(player.player_id == 1);
        REQUIRE(player.name == "player");
        REQUIRE(player.energy == 2);
        const auto &factory = player.factories.front();
        REQUIRE(player.factories.front().x == 3);
        REQUIRE(player.factories.front().y == 4);
        REQUIRE(player.entities.empty());
    }
}

SCENARIO("Player operators are semantically correct", "[player_operators]") {
    GIVEN("Two players") {
        auto player1 = Player(0, "player0", "");
        auto player2 = Player(1, "player1", "");
        auto player3 = Player(0, "player0", "");
        WHEN("players are compared for equality") {
            THEN("equal players are equal") {
                REQUIRE(player1 == player1);
                REQUIRE(player2 == player2);
                REQUIRE(player3 == player1);
                REQUIRE(player1 == player3);
            }
            THEN("unequal players are unequal") {
                REQUIRE(!(player1 == player2));
                REQUIRE(!(player2 == player1));
                REQUIRE(!(player3 == player2));
            }
        }
        WHEN("players are compared by order") {
            THEN("the ordering relation is correct") {
                REQUIRE(player1 < player2);
                REQUIRE(!(player2 < player1));
                REQUIRE(player3 < player2);
                REQUIRE(!(player3 < player1));
            }
        }
        WHEN("equal players are hashed") {
            auto hash1 = std::hash<Player>()(player1);
            auto hash2 = std::hash<Player>()(player3);
            THEN("the hashes are equal") {
                REQUIRE(hash1 == hash2);
            }
        }
    }
}

SCENARIO("Players are encoded to JSON and bot serial format correctly", "[player_serial]") {
    GIVEN("A player") {
        auto entity = make_entity<PlayerEntity>(1, 128);
        Player player(1, "player", 2, {{3, 4}}, {{{0, 0}, entity}});
        WHEN("player is serialized to JSON") {
            nlohmann::json json;
            to_json(json, player);
            THEN("contents of JSON are correct") {
                REQUIRE(json.at("player_id") == player.player_id);
                REQUIRE(json.at("name") == player.name);
                REQUIRE(json.at("energy") == player.energy);
                REQUIRE(json.at("factory_location") == player.factories.front());
                REQUIRE(json.at("entities").size() == 1);
                REQUIRE(json.at("entities")[0].at("x") == 0);
                REQUIRE(json.at("entities")[0].at("y") == 0);
                REQUIRE(json.at("entities")[0].at("energy") == 128);
            }
        }
        WHEN("player is serialized to bot serial format") {
            std::stringstream stream;
            stream << player;
            THEN("output is correct") {
                std::stringstream stream2;
                stream2 << *entity;
                REQUIRE(stream.str() == "1 1 2\n0 0 " + stream2.str());
            }
        }
    }
}

SCENARIO("Players and entities interact correctly", "[player_entities]") {
    hlt::PlayerFactory player_factory{};
    Player player = player_factory.new_player("./bot");
    WHEN("entity added") {
        const auto BASE_ENERGY = 100;
        std::shared_ptr<hlt::PlayerEntity> entity = hlt::make_entity<hlt::PlayerEntity>(player.player_id, BASE_ENERGY);
        Location location = {0, 0};
        player.add_entity(location, entity);
        THEN("entity is stored by player, has correct energy") {
            REQUIRE(player.entities[location] == entity);
            REQUIRE(player.entities[location]->energy == BASE_ENERGY);
            REQUIRE(player.entities[location]->owner_id == player.player_id);
        }
        WHEN("additional entity for same player added to same cell") {
            player.add_entity(location, entity);
            THEN("entities do not merge (merges hidden by transaction abstraction)") {
                REQUIRE(player.entities[location]->energy == BASE_ENERGY);
            }
        }
        WHEN("entity at different location added") {
            Location location2 = {1, 1};
            std::shared_ptr<hlt::PlayerEntity> entity2 = hlt::make_entity<hlt::PlayerEntity>(player.player_id, BASE_ENERGY);
            player.add_entity(location2, entity2);
            THEN ("both entities exist with original energy") {
                REQUIRE(player.entities.size() == 2);
                REQUIRE(player.entities[location] == entity);
                REQUIRE(player.entities[location]->energy == BASE_ENERGY);
                REQUIRE(player.entities[location]->owner_id == player.player_id);

                REQUIRE(player.entities[location2] == entity2);
                REQUIRE(player.entities[location2]->energy == BASE_ENERGY);
                REQUIRE(player.entities[location2]->owner_id == player.player_id);
            }
            WHEN("one entity is deleted") {
                player.remove_entity(location);
                THEN ("entity is removed, other entity still there") {
                    REQUIRE(player.entities.size() == 1);
                    REQUIRE(player.entities[location2] == entity2);
                }
            }
        }
    }
}

SCENARIO("Player factory initializes player IDs", "[player_factory]") {
    GIVEN("A new player factory") {
        PlayerFactory factory;
        WHEN("a player is created") {
            Player player = factory.new_player("./bot");
            THEN("the new player is initialized correctly") {
                REQUIRE(player.player_id == 0);
                REQUIRE(player.name == "0");
                REQUIRE(player.command == "./bot");
            }
        }
        WHEN("another player is created") {
            factory.new_player("./bot");
            Player player = factory.new_player("./bot2");
            THEN("the second player is initialized correctly") {
                REQUIRE(player.player_id == 1);
                REQUIRE(player.name == "1");
                REQUIRE(player.command == "./bot2");
            }
        }
    }
}
