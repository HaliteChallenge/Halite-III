#include "Cell.hpp"
#include "JsonError.hpp"

/** The JSON key for cell type. */
constexpr auto JSON_TYPE_KEY = "type";
/** The JSON key for production. */
constexpr auto JSON_PRODUCTION_KEY = "production";
/** The JSON key for energy factor. */
constexpr auto JSON_ENERGY_FACTOR_KEY = "energy_factor";

namespace hlt {

/**
 * Convert a Cell to JSON format.
 * @param[out] json The output JSON.
 * @param cell The cell to convert.
 */
void to_json(nlohmann::json &json, const Cell &cell) { cell->to_json(json); }

/**
 * Convert an encoded Cell from JSON format.
 * @param json The JSON.
 * @param[out] cell The converted cell.
 */
void from_json(const nlohmann::json &json, Cell &cell) {
    // The type field determines the Cell subclass that will be instantiated.
    const std::string &type = json.at(JSON_TYPE_KEY);
    if (type == NormalCell::CELL_TYPE_NAME) {
        cell = make_cell<NormalCell>(json);
    } else if (type == ObstacleCell::CELL_TYPE_NAME) {
        cell = make_cell<ObstacleCell>(json);
    } else if (type == EnergyFactorCell::CELL_TYPE_NAME) {
        cell = make_cell<EnergyFactorCell>(json);
    } else if (type == FactoryCell::CELL_TYPE_NAME) {
        cell = make_cell<FactoryCell>(json);
    } else {
        throw JsonError(json);
    }
}

/**
 * Write a Cell to bot serial format.
 * @param ostream The output stream.
 * @param cell The cell to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Cell &cell) {
    return ostream << cell->to_bot_serial() << std::endl;
}

/**
 * Add an entity by player, possibly merging with an existing entity.
 * @param player The player for the entity.
 * @param entity The entity to add.
 */
void BaseCell::add_entity(const Player &player, std::shared_ptr<Entity> &entity) {
    auto entity_iterator = entities.find(player.player_id);
    if (entity_iterator != entities.end()) {
        // If the player already has an entity there, merge
        entity_iterator->second->energy += entity->energy;
    } else {
        // Otherwise, move this entity there
        entities[player.player_id] = entity;
    }
}

/**
 * Remove an entity by player.
 * @param player The player of the entity.
 */
void BaseCell::remove_entity(const Player &player) {
    entities.erase(player.player_id);
}

/**
 * Create ProductionCell from JSON.
 * @param json The JSON.
 */
ProductionCell::ProductionCell(const nlohmann::json &json) : _production(json.at(JSON_PRODUCTION_KEY)) {}

/**
 * Convert a NormalCell to JSON format.
 * @param[out] json The JSON output.
 */
void NormalCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,       CELL_TYPE_NAME},
            {JSON_PRODUCTION_KEY, production()}};
}

/**
 * Convert a NormalCell to bot serial format.
 * @return The formatted output.
 */
std::string NormalCell::to_bot_serial() const {
    return std::string(CELL_TYPE_NAME) + " " + std::to_string(production());
}

/**
 * Convert an ObstacleCell to JSON format.
 * @param[out] json The JSON output.
 */
void ObstacleCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,       CELL_TYPE_NAME},
            {JSON_PRODUCTION_KEY, production()}};
}

/**
 * Convert a ObstacleCell to bot serial format.
 * @return The formatted output.
 */
std::string ObstacleCell::to_bot_serial() const {
    return std::string(CELL_TYPE_NAME) + " " + std::to_string(production());
}

/**
 * Convert an EnergyFactorCell to JSON format.
 * @param[out] json The JSON output.
 */
void EnergyFactorCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,          CELL_TYPE_NAME},
            {JSON_PRODUCTION_KEY,    production()},
            {JSON_ENERGY_FACTOR_KEY, _energy_factor}};
}

/**
 * Convert a EnergyFactorCell to bot serial format.
 * @return The formatted output.
 */
std::string EnergyFactorCell::to_bot_serial() const {
    return std::string(JSON_ENERGY_FACTOR_KEY) + " " +
           std::to_string(production()) + " " +
           std::to_string(_energy_factor);
}

/**
 * Create EnergyFactorCell from JSON.
 * @param json The JSON.
 */
EnergyFactorCell::EnergyFactorCell(const nlohmann::json &json) :
        ProductionCell(json), _energy_factor(json.at(JSON_ENERGY_FACTOR_KEY)) {}

/**
 * Convert a FactoryCell to JSON format.
 * @param[out] json The JSON output.
 */
void FactoryCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY, CELL_TYPE_NAME}};
}

/**
 * Convert a FactoryCell to bot serial format.
 * @return The formatted output.
 */
std::string FactoryCell::to_bot_serial() const { return CELL_TYPE_NAME; }

}
