#include "catch.hpp"

#include "nlohmann/json.hpp"

#include "Replay.hpp"
#include "Generator.hpp"
#include "FractalValueNoiseTileGenerator.hpp"

using namespace hlt;

TEST_CASE("Replay object created and converted to json as expected by visualizer") {

    // Create players for replay struct
    size_t NUMBER_OF_PLAYERS = 2;
    PlayerFactory factory{};
    Player player1 = factory.new_player("./bot");
    Player player2 = factory.new_player("./bot");

    std::vector<Player> players = {player1, player2};

    const auto TOTAL_TURNS = 30;
    GameStatistics stats{};
    stats.number_turns = TOTAL_TURNS;

    // Seed randomness for creating productions
    const auto SEED = 10;
    srand(SEED);

    const Constants &GAME_CONSTANTS = Constants::get();
    // Add player to stats struct and make up some stats
    for (const auto player : players) {
        stats.player_statistics.emplace_back(player.player_id);
        auto total_production = 0;
        for (int i = 0; i < TOTAL_TURNS; ++i) {
            auto turn_production = rand() % (GAME_CONSTANTS.MAX_CELL_PRODUCTION - GAME_CONSTANTS.MIN_CELL_PRODUCTION) +
                                   GAME_CONSTANTS.MAX_CELL_PRODUCTION;
            stats.player_statistics.back().turn_productions.push_back(turn_production);
            total_production += turn_production;
            stats.player_statistics.back().last_turn_alive = i;
        }
        stats.player_statistics.back().total_production = total_production;
    }

    // Rank players from pretend productions
    std::stable_sort(stats.player_statistics.begin(), stats.player_statistics.end());
    // Reverse list to have best players first
    std::reverse(stats.player_statistics.begin(), stats.player_statistics.end());

    for (size_t index = 0; index < stats.player_statistics.size(); ++index) {
        stats.player_statistics[index].rank = index + 1l;
    }

    // Create game map
    const auto MAP_WIDTH = GAME_CONSTANTS.DEFAULT_MAP_WIDTH;
    const auto MAP_HEIGHT = GAME_CONSTANTS.DEFAULT_MAP_HEIGHT;

    // Set parameters for replay struct
    mapgen::MapParameters params = {mapgen::MapType::Fractal, SEED, MAP_WIDTH, MAP_HEIGHT, NUMBER_OF_PLAYERS};
    std::unique_ptr<mapgen::Generator> generator = std::make_unique<mapgen::FractalValueNoiseTileGenerator>(params);

    // Save space for map, factories
    Map game_map{MAP_WIDTH, MAP_HEIGHT};
    std::vector<Location> factories;
    factories.reserve(players.size());

    // Generate map, place factories, add an entity at factory for each player
    generator->generate(game_map, factories);
    std::unordered_map<Player::id_type, Player> player_map;
    for (auto &player : players) {
        player.factories.push_back(factories.back());
        factories.pop_back();
        player.entities[player.factories.front()] = make_entity<Entity>(player.player_id,
                                                                        GAME_CONSTANTS.NEW_ENTITY_ENERGY);
        player_map[player.player_id] = player;
    }

    // Create replay struct
    Replay replay{stats, NUMBER_OF_PLAYERS, SEED, game_map};

    replay.players = player_map;

    // Add events for every turn (randomly)
    for (int i = 0; i < TOTAL_TURNS; ++i) {
        replay.full_frames.emplace_back();
        for (const auto player : players) {
            // create move in random location
            const auto x = rand() % MAP_WIDTH;
            const auto y = rand() % MAP_HEIGHT;
            Command new_command;
            switch (i % 4) {
            case 0:
                new_command = std::make_shared<MoveCommand>(x, y, Direction::North);
                break;
            case 1:
                new_command = std::make_shared<MoveCommand>(x, y, Direction::East);
                break;
            case 2:
                new_command = std::make_shared<MoveCommand>(x, y, Direction::South);
                break;
            case 3:
                new_command = std::make_shared<MoveCommand>(x, y, Direction::West);
                break;
            }
            replay.full_frames.back().moves[player.player_id] = {new_command};

            // Also add game_event
            Location location{x, y};
            switch (i % 3) {
            case 0:
                replay.full_frames.back().events.push_back(
                        std::make_unique<SpawnEvent>(location, GAME_CONSTANTS.NEW_ENTITY_ENERGY, player.player_id));
                break;
            case 1:
                replay.full_frames.back().events.push_back(std::make_unique<DeathEvent>(location, player.player_id));
                break;
            }
        }
    }


    SECTION("Replay converted to json") {
        nlohmann::json replay_json;
        nlohmann::to_json(replay_json, replay);

        // Test map width and height exist in json, are same as expected
        SECTION("Map json format") {
            REQUIRE(replay_json["production_map"]["width"].get<dimension_type>() == MAP_WIDTH);
            REQUIRE(replay_json["production_map"]["height"].get<dimension_type>() == MAP_HEIGHT);
            REQUIRE_NOTHROW(replay_json["production_map"].at("grid"));

            for (dimension_type row = 0; row < MAP_HEIGHT; ++row) {
                for (dimension_type col = 0; col < MAP_WIDTH; ++col) {
                    REQUIRE_NOTHROW(replay_json["production_map"]["grid"].at(row).at(col));
                    REQUIRE_NOTHROW(replay_json["production_map"]["grid"].at(row).at(col).at("type"));
                    if (replay_json["production_map"]["grid"][row][col]["type"] == "n") {
                        REQUIRE_NOTHROW(replay_json["production_map"]["grid"][row][col].at("production"));
                        REQUIRE(replay_json["production_map"]["grid"][row][col].at("production") ==
                                replay.production_map.at(col, row)->production());
                    }
                }
            }

        }

        // Expect game constants labeled as such
        SECTION("Game constants used by replay") {
            REQUIRE_NOTHROW(replay_json.at("GAME_CONSTANTS"));
            REQUIRE_NOTHROW(replay_json["GAME_CONSTANTS"].at("BASE_TURN_ENERGY_LOSS"));
        }
        SECTION("Player json format") {
            REQUIRE(replay_json.at("players").size() == NUMBER_OF_PLAYERS);
            for (const auto player_json : replay_json["players"]) {
                REQUIRE_NOTHROW(player_json.at("player_id").get<Player::id_type>());
                Player::id_type player_id = player_json.at("player_id").get<Player::id_type>();
                REQUIRE(player_json.at("factory_location").at("x") == player_map[player_id].factories.front().x);
                REQUIRE(player_json.at("factory_location").at("y") == player_map[player_id].factories.front().y);
            }

            SECTION("Entity json format") {
                for (const auto player_json : replay_json["players"]) {
                    REQUIRE_NOTHROW(player_json.at("entities"));
                    const auto entities_json = player_json["entities"];
                    for (nlohmann::json entity : entities_json) {
                        REQUIRE_NOTHROW(entity.at("x"));
                        REQUIRE_NOTHROW(entity.at("y"));
                        REQUIRE_NOTHROW(entity.at("energy"));
                        Location entity_location{entity.at("x").get<dimension_type>(),
                                                 entity.at("y").get<dimension_type>()};
                        REQUIRE(entity.at("energy").get<energy_type>() == players[player_json.at(
                                "player_id").get<Player::id_type>()].entities[entity_location]->energy);
                    }
                }
            }
        }

        SECTION("Frames format, length") {
            REQUIRE_NOTHROW(replay_json.at("full_frames"));
            REQUIRE(replay_json.at("full_frames").size() == stats.number_turns);

            SECTION("Turns format") {
                bool saw_spawn = false;
                bool saw_death = false;
                bool saw_north, saw_south, saw_east, saw_west = false;
                for (size_t i = 0; i < TOTAL_TURNS; ++i) {
                    // Check events formatted as expected
                    REQUIRE_NOTHROW(replay_json.at("full_frames").at(i).at("events"));
                    const auto events_json = replay_json["full_frames"].at(i)["events"];
                    REQUIRE(events_json.size() == replay.full_frames[i].events.size());
                    for (size_t event_idx = 0; event_idx < events_json.size(); ++event_idx) {
                        REQUIRE_NOTHROW(events_json.at(event_idx).at("type"));
                        if (events_json[event_idx]["type"] == "death") {
                            // must also be location, owner_id
                            REQUIRE_NOTHROW(events_json[event_idx].at("location"));
                            REQUIRE_NOTHROW(events_json[event_idx]["location"].at("x"));
                            REQUIRE_NOTHROW(events_json[event_idx]["location"].at("y"));
                            REQUIRE_NOTHROW(events_json[event_idx].at("owner_id"));
                            saw_death = true;

                        }
                        if (events_json[event_idx]["type"] == "spawn") {
                            // must also be location, owner_id, energy
                            REQUIRE_NOTHROW(events_json[event_idx].at("location"));
                            REQUIRE_NOTHROW(events_json[event_idx]["location"].at("x"));
                            REQUIRE_NOTHROW(events_json[event_idx]["location"].at("y"));
                            REQUIRE_NOTHROW(events_json[event_idx].at("owner_id"));
                            REQUIRE_NOTHROW(events_json[event_idx].at("energy"));
                            saw_spawn = true;
                        }
                    }

                    // Check moves formatted as expected
                    REQUIRE_NOTHROW(replay_json.at("full_frames").at(i).at("moves"));
                    const auto moves_json = replay_json["full_frames"].at(i)["moves"];
                    REQUIRE(moves_json.size() == replay.full_frames[i].moves.size());

                    for (const auto player_moves_json : moves_json) {
                        for (const auto move_json : player_moves_json) {
                            INFO(move_json);
                            REQUIRE_NOTHROW(move_json.at("type"));
                            if (move_json["type"] == "move") {
                                REQUIRE_NOTHROW(move_json.at("entity_x"));
                                REQUIRE_NOTHROW(move_json.at("entity_y"));
                                REQUIRE_NOTHROW(move_json.at("direction"));
                                const auto direction = move_json["direction"].get<std::string>();
                                const auto DIRECTION_LENGTH = 1;
                                if (direction == to_string(static_cast<char>(Direction::North))) saw_north = true;
                                if (direction == to_string(static_cast<char>(Direction::East))) saw_east = true;
                                if (direction == to_string(static_cast<char>(Direction::South))) saw_south = true;
                                if (direction == to_string(static_cast<char>(Direction::West))) saw_west = true;
                            }
                        }
                    }
                }
                REQUIRE(saw_spawn);
                REQUIRE(saw_death);
                REQUIRE(saw_north);
                REQUIRE(saw_south);
                REQUIRE(saw_east);
                REQUIRE(saw_west);
            }
        }
    }
}
