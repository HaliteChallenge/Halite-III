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
//#include <tclap/CmdLine.h>


//namespace TCLAP {
//    template<>
//    struct ArgTraits<std::pair<signed int, signed int> > {
//        typedef TCLAP::ValueLike ValueCategory;
//    };
//}
//
//bool quiet_output =
//        false; //Need to be passed to a bunch of classes; extern is cleaner.
//
//bool always_log =
//        false; //Flag to always log game state (regardless of whether bots are error-ing out)
//
//void promptDimensions(unsigned short& w, unsigned short& h);

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
        // will these vectors get removed because created in local scope?
        std::list<hlt::Player> game_size_player_vector;
        for (unsigned long player_name_idx = 0; player_name_idx < num_players; ++player_name_idx){
            hlt::Player new_player = player_creator.new_player(player_names.at(player_name_idx));
            game_size_player_vector.push_back(new_player);
        }
        player_lists.push_back(game_size_player_vector);
    }


    //Create game. Null parameters will be ignored.
    // Generate a tile map class to create maps from
    hlt::Map::dimension_type map_width = 64;
    hlt::Map::dimension_type map_height = 64;

    hlt::mapgen::MapParameters map_params1 = {hlt::mapgen::MapType::Tile, seed, map_width, map_height, game_sizes[0]};
    hlt::mapgen::MapParameters map_params2 = {hlt::mapgen::MapType::Tile, seed, map_width, map_height, game_sizes[1]};
    hlt::mapgen::MapParameters map_params4 = {hlt::mapgen::MapType::Tile, seed, map_width, map_height, game_sizes[2]};

    hlt::mapgen::TileGenerator generator1(map_params1);
    hlt::mapgen::TileGenerator generator2(map_params2);
    hlt::mapgen::TileGenerator generator4(map_params4);

    //hlt::Map new_map = generator1.generate(player_lists[0]);

    std::vector<hlt::mapgen::TileGenerator> generators = {generator1, generator2, generator4};

    std::vector<hlt::Map> maps;
    for (int gen_idx = 0; gen_idx < generators.size(); gen_idx++) {
        hlt::Map new_map = generators[gen_idx].generate(player_lists[gen_idx]);
        maps.push_back(std::move(new_map));
    }
    for (auto &map : maps){
        std::cout << "New map \n \n";
        std::cout << map;
    }


    return 0;
}
