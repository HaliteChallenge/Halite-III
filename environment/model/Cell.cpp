#include "Cell.hpp"

namespace hlt {

void to_json(nlohmann::json &json, const Cell &cell) { cell->to_json(json); }

void from_json(const nlohmann::json &json, Cell &cell) {
    const auto &type = json.at("type").get<std::string>();
    if (type == "normal") {
        cell = std::make_unique<NormalCell>(json);
    } else if (type == "obstacle") {
        cell = std::make_unique<ObstacleCell>(json);
    } else if (type == "energy_factor") {
        cell = std::make_unique<EnergyFactorCell>(json);
    } else if (type == "factory") {
        cell = std::make_unique<FactoryCell>(json);
    } else {
        // TODO: error case
    }
}

std::ostream &operator<<(std::ostream &ostream, const Cell &cell) {
    return ostream << cell->to_bot_serial() << std::endl;
}

ProductionCell::ProductionCell(const nlohmann::json &json) :
        _production(json.at("production").get<decltype(ProductionCell::_production)>()) {}

void NormalCell::to_json(nlohmann::json &json) const {
    json = {{"type",       "normal"},
            {"production", production()}};
}

std::string NormalCell::to_bot_serial() const {
    return "normal " + std::to_string(production());
}

void ObstacleCell::to_json(nlohmann::json &json) const {
    json = {{"type",       "obstacle"},
            {"production", production()}};
}

std::string ObstacleCell::to_bot_serial() const {
    return "obstacle " + std::to_string(production());
}

void EnergyFactorCell::to_json(nlohmann::json &json) const {
    json = {{"type",          "energy_factor"},
            {"production",    production()},
            {"energy_factor", _energy_factor}};
}

std::string EnergyFactorCell::to_bot_serial() const {
    return "energy_factor " + std::to_string(production()) + " " + std::to_string(_energy_factor);
}

EnergyFactorCell::EnergyFactorCell(const nlohmann::json &json) :
        ProductionCell(json),
        _energy_factor(json.at("energy_factor").get<decltype(EnergyFactorCell::_energy_factor)>()) {}

void FactoryCell::to_json(nlohmann::json &json) const {
    json = {"type", "factory"};
}

std::string FactoryCell::to_bot_serial() const { return "factory"; }

}
