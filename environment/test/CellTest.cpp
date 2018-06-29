#include "Cell.hpp"

#include "nlohmann/json.hpp"

#include "catch.hpp"

using namespace hlt;

SCENARIO("Cell constructor initializes member variables", "[cell]") {
    GIVEN("A normal cell with production") {
        const auto BASE_PRODUCTION = 10;
        hlt::Cell normal_cell = hlt::make_cell<hlt::NormalCell>(BASE_PRODUCTION);

        REQUIRE(normal_cell->production() == BASE_PRODUCTION);
        REQUIRE(normal_cell->is_passable());
        REQUIRE(normal_cell->energy_factor() == 0);
        WHEN("converted to json") {
            nlohmann::json json;
            to_json(json, normal_cell);
            THEN("json has type and production of correct values") {
                REQUIRE(json.at("type") == "n");
                REQUIRE(json.at("production") == BASE_PRODUCTION);
            }
        }
        WHEN("converted to bot serial format") {
            std::stringstream stream;
            stream << normal_cell;
            THEN("output has type and production of correct values") {
                REQUIRE(stream.str() == "n 10\n");
            }
        }
        WHEN("entity added") {
            hlt::PlayerFactory player_factory{};
            Player player = player_factory.new_player("./bot");
            const auto BASE_ENERGY = 100;
            std::shared_ptr<hlt::Entity> entity = hlt::make_entity<hlt::Entity>(player.player_id, BASE_ENERGY);
            normal_cell->add_entity(player, entity);
            THEN("entity is stored by cell, has correct energy, owner") {
                REQUIRE(normal_cell->entities[player.player_id] == entity);
                REQUIRE(normal_cell->entities[player.player_id]->energy == BASE_ENERGY);
                REQUIRE(normal_cell->entities[player.player_id]->owner_id == player.player_id);
            }
            WHEN("additional entity for same player added to same cell") {
                normal_cell->add_entity(player, entity);
                THEN("entities merge, have additive energy") {
                    REQUIRE(normal_cell->entities[player.player_id]->energy == 2 * BASE_ENERGY);
                }
            }
            WHEN("different player's entity added") {
                Player player2 = player_factory.new_player("./bot2");
                std::shared_ptr<hlt::Entity> entity2 = hlt::make_entity<hlt::Entity>(player2.player_id, BASE_ENERGY);
                normal_cell->add_entity(player2, entity2);
                THEN ("both entities exist with original energy") {
                    REQUIRE(normal_cell->entities.size() == 2);
                    REQUIRE(normal_cell->entities[player.player_id] == entity);
                    REQUIRE(normal_cell->entities[player.player_id]->energy == BASE_ENERGY);
                    REQUIRE(normal_cell->entities[player.player_id]->owner_id == player.player_id);

                    REQUIRE(normal_cell->entities[player2.player_id] == entity2);
                    REQUIRE(normal_cell->entities[player2.player_id]->energy == BASE_ENERGY);
                    REQUIRE(normal_cell->entities[player2.player_id]->owner_id == player2.player_id);
                }
                WHEN("one entity is deleted") {
                    normal_cell->remove_entity(player);
                    THEN ("entity is removed, other player's entity still there") {
                        REQUIRE(normal_cell->entities.size() == 1);
                        REQUIRE(normal_cell->entities[player2.player_id] == entity2);
                    }
                }
            }
        }
    }
    GIVEN("A factory cell") {
        Cell factory_cell = make_cell<FactoryCell>();
        REQUIRE(factory_cell->production() == 0);

        WHEN("converted to json") {
            nlohmann::json json;
            to_json(json, factory_cell);
            THEN("json has type and production of correct values") {
                REQUIRE(json.at("type") == "f");
                // factory cells don't have production, so not included in json
                REQUIRE_THROWS(json.at("production"));
            }
        }
        WHEN("converted to bot serial format") {
            std::stringstream stream;
            stream << factory_cell;
            THEN("output has type of correct value") {
                REQUIRE(stream.str() == "f\n");
            }
        }
    }
    GIVEN("An obstacle cell") {
        const auto BASE_PRODUCTION = 10;
        hlt::Cell obstacle_cell = hlt::make_cell<hlt::ObstacleCell>(BASE_PRODUCTION);

        REQUIRE(obstacle_cell->production() == BASE_PRODUCTION);
        REQUIRE(!obstacle_cell->is_passable());
        REQUIRE(obstacle_cell->energy_factor() == 0);
        WHEN("converted to json") {
            nlohmann::json json;
            to_json(json, obstacle_cell);
            THEN("json has type and production of correct values") {
                REQUIRE(json.at("type") == "o");
                REQUIRE(json.at("production") == BASE_PRODUCTION);
            }
        }
        WHEN("converted to bot serial format") {
            std::stringstream stream;
            stream << obstacle_cell;
            THEN("output has type and production of correct values") {
                REQUIRE(stream.str() == "o 10\n");
            }
        }
    }
    GIVEN("An energy factor cell") {
        const auto BASE_PRODUCTION = 10;
        const auto ENERGY_FACTOR = 2;
        hlt::Cell energy_factor_cell = hlt::make_cell<hlt::EnergyFactorCell>(BASE_PRODUCTION, ENERGY_FACTOR);

        REQUIRE(energy_factor_cell->production() == BASE_PRODUCTION);
        REQUIRE(energy_factor_cell->is_passable());
        REQUIRE(energy_factor_cell->energy_factor() == ENERGY_FACTOR);
        WHEN("converted to json") {
            nlohmann::json json;
            to_json(json, energy_factor_cell);
            THEN("json has type and production of correct values") {
                REQUIRE(json.at("type") == "e");
                REQUIRE(json.at("production") == BASE_PRODUCTION);
                REQUIRE(json.at("energy_factor") == ENERGY_FACTOR);
            }
        }
        WHEN("converted to bot serial format") {
            std::stringstream stream;
            stream << energy_factor_cell;
            THEN("output has type, production, and energy factor of correct values") {
                REQUIRE(stream.str() == "e 10 2\n");
            }
        }
    }
}
