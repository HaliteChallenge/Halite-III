//
// Created by Katherine Binney on 6/11/18.
//

#include "MapConstants.hpp"

auto hlt::MapConstants::to_json() const -> nlohmann::json {
    return {
        { "MAX_CELL_PRODUCTION", MAX_CELL_PRODUCTION },
        { "MIN_CELL_PRODUCTION", MIN_CELL_PRODUCTION },
        { "MAX_ENERGY", MAX_ENERGY },
        { "BLUR_FACTOR", BLUR_FACTOR },

    };
}

auto hlt::MapConstants::from_json(const nlohmann::json& json) -> void {
    MAX_CELL_PRODUCTION = json.value("MAX_CELL_PRODUCTION", MAX_CELL_PRODUCTION);
    MIN_CELL_PRODUCTION = json.value("MIN_CELL_PRODUCTION", MIN_CELL_PRODUCTION);
    MAX_ENERGY = json.value("MAX_ENERGY", MAX_ENERGY);
    BLUR_FACTOR = json.value("BLUR_FACTOR", BLUR_FACTOR);
}
