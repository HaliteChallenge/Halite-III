//
// Created by Katherine Binney on 6/11/18.
//

#ifndef MAP_CONSTANTS_HPP
#define MAP_CONSTANTS_HPP

#include "../util/json.hpp"

namespace hlt {

    /**
     * Map Generation constants that may be tweaked (though they should be at their
     * default values in a tournament setting).
     */
    struct MapConstants {
        // Halite 3 constants!
        long MAX_CELL_PRODUCTION = 255;
        long MIN_CELL_PRODUCTION = 1;
        long MAX_ENERGY = 255;
        double BLUR_FACTOR = 0.75;


        static auto get_mut() -> MapConstants& {
            // Guaranteed initialized only once by C++11
            static MapConstants instance;
            return instance;
        }

        static auto get() -> const MapConstants& {
            return get_mut();
        }

        auto to_json() const -> nlohmann::json;
        auto from_json(const nlohmann::json& json) -> void;
    };
}

#endif //MAP_CONSTANTS_HPP
