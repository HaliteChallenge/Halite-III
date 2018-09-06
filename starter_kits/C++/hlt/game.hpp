#pragma once

#include "game_map.hpp"
#include "player.hpp"
#include "types.hpp"

#include <vector>
#include <iostream>

namespace hlt {
    struct Game {
        int turn_number;
        PlayerId my_id;
        std::vector<Player*> players;
        Player* me;
        GameMap* game_map;

        Game();
        void ready(const std::string& name);
        void update_frame();
        bool end_turn(const std::vector<Command>& commands);
    };
}
