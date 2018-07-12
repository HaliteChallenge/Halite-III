#include "Cell.hpp"
#include "JsonError.hpp"

/** The JSON key for cell type. */
constexpr auto JSON_TYPE_KEY = "type";
/** The JSON key for energy. */
constexpr auto JSON_ENERGY_KEY = "energy";

namespace hlt {

/**
 * Convert a Cell to JSON format.
 * @param[out] json The output JSON.
 * @param cell The cell to convert.
 */
void to_json(nlohmann::json &json, const std::unique_ptr<Cell> &cell) { cell->to_json(json); }

/**
 * Write a Cell to bot serial format.
 * @param ostream The output stream.
 * @param cell The cell to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Cell &cell) {
    return ostream << cell.to_bot_serial() << std::endl;
}

/**
 * Convert a NormalCell to JSON format.
 * @param[out] json The JSON output.
 */
void NormalCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,   Name::Normal},
            {JSON_ENERGY_KEY, energy()}};
}

/**
 * Convert a NormalCell to bot serial format.
 * @return The formatted output.
 */
std::string NormalCell::to_bot_serial() const {
    return to_string(Name::Normal) + " " + std::to_string(energy());
}

/**
 * Convert an ObstacleCell to JSON format.
 * @param[out] json The JSON output.
 */
void ObstacleCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,   Name::Obstacle},
            {JSON_ENERGY_KEY, energy()}};
}

/**
 * Convert a ObstacleCell to bot serial format.
 * @return The formatted output.
 */
std::string ObstacleCell::to_bot_serial() const {
    return to_string(Name::Obstacle) + " " + std::to_string(energy());
}

/**
 * Convert a FactoryCell to JSON format.
 * @param[out] json The JSON output.
 */
void FactoryCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY, Name::Factory}};
}

/**
 * Convert a FactoryCell to bot serial format.
 * @return The formatted output.
 */
std::string FactoryCell::to_bot_serial() const { return to_string(Name::Factory); }

}
