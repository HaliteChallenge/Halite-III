#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <iostream>
#include <list>
#include <vector>

#include "TileGenerator.hpp"
#include "../model/Player.hpp"
#include "MapConstants.hpp"
#include "../model/Map.hpp"
#include "BlurTileGenerator.hpp"

void draw_map(hlt::Map &map);

int main(int argc, char** argv) {
    srand(time(NULL)); //For all non-seeded randomness.

    unsigned int seed = (unsigned int)
                (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
                 % 4294967295);

    // Player Factory
    // For testing, create dummy players, and lists of players of lengths 1, 2, and 4
    hlt::PlayerFactory player_creator;
    std::vector<std::string> player_names = {"Player1", "Player2", "Player3", "Player4"};

    player_creator.new_player("Hello");

    std::vector<unsigned long> game_sizes = {1, 2, 4};

    std::vector<std::list<hlt::Player>> player_lists;

    for (auto num_players : game_sizes) {
        std::list<hlt::Player> game_size_player_vector;
        for (unsigned long player_name_idx = 0; player_name_idx < num_players; ++player_name_idx){
            hlt::Player new_player = player_creator.new_player(player_names.at(player_name_idx));
            game_size_player_vector.push_back(new_player);
        }
        player_lists.push_back(game_size_player_vector);
    }

    // Generate tile map classes to test map generation
    // For initial testing, use square maps
    hlt::Map::dimension_type map_width = 32;
    hlt::Map::dimension_type map_height = 32;

    // Try generating maps of the same size for different numbers of players (ie so tile size will differ)
    hlt::mapgen::MapParameters map_params1 = {hlt::mapgen::MapType::Tile, seed, map_width, map_height, game_sizes[0]};
    hlt::mapgen::MapParameters map_params2 = {hlt::mapgen::MapType::Tile, seed, map_width, map_height, game_sizes[1]};
    hlt::mapgen::MapParameters map_params4 = {hlt::mapgen::MapType::Tile, seed, map_width, map_height, game_sizes[2]};

    hlt::mapgen::BlurTileGenerator generator1(map_params1);
    hlt::mapgen::BlurTileGenerator generator2(map_params2);
    hlt::mapgen::BlurTileGenerator generator4(map_params4);

    std::vector<hlt::mapgen::BlurTileGenerator> generators = {generator1, generator2, generator4};

    // Finally, generate a vector of maps, with each generation function being given a list of players of the
    // correct length, then print these maps for manual review
    std::vector<hlt::Map> maps;
    for (int gen_idx = 0; gen_idx < generators.size(); gen_idx++) {
        hlt::Map new_map = generators[gen_idx].generate(player_lists[gen_idx]);
        maps.push_back(std::move(new_map));
    }
    for (auto &map : maps){
        std::cout << "New Map \n Dimensions " << map.height << " " << map.width << std::endl;
        draw_map(map);
        std::cout << "\n \n";
    }

    return 0;
}
/**
 * Print a halite map in non-game standard format
 * Will print the map to cout as a grid of production values
 * Factories are represented as x's
 * @param map: the map to be printed
 */
void draw_map(hlt::Map &map) {
    for (auto &row : map.grid) {
        for (auto &cell : row) {
            if (cell -> production() == 0) {
                std::cout << std::setw(5) << "X";
            }
            else {
                std::cout << std::setw(5) << cell->production();
            }
        }
        std::cout << std::endl;
    }
}