#ifndef CELL_HPP
#define CELL_HPP

#include <iostream>
#include <unordered_map>

#include "Constants.hpp"
#include "Entity.hpp"
#include "Player.hpp"

#include "nlohmann/json_fwd.hpp"

namespace hlt {

class BaseCell;

/** Container type for cells, exposed to outside users like Map. */
using Cell = std::unique_ptr<BaseCell>;

/**
 * Factory method for cells to hide the constructor from clients.
 * Ideally this function would return Cell, and it will always be instantiated to permit that,
 * but that is not type-safe.
 *
 * @tparam CellType The class of the cell.
 * @tparam Args The types of the arguments to the cell constructor.
 * @param args The arguments to the cell constructor.
 * @return The newly constructed cell.
 */
template<class CellType, typename... Args>
std::unique_ptr<CellType> make_cell(Args &&... args) {
    static_assert(std::is_base_of<BaseCell, CellType>::value, "make_cell requires a BaseCell subclass");
    return std::make_unique<CellType>(std::forward<Args>(args)...);
}

/**
 * Convert a Cell to JSON format.
 * @param[out] json The output JSON.
 * @param cell The cell to convert.
 */
void to_json(nlohmann::json &json, const Cell &cell);

/**
 * Convert an encoded Cell from JSON format.
 * @param json The JSON.
 * @param[out] cell The converted cell.
 */
void from_json(const nlohmann::json &json, Cell &cell);

/**
 * Write a Cell to bot serial format.
 * @param ostream The output stream.
 * @param cell The cell to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Cell &cell);

/** Abstract cell data type. Subclasses are intended to be data-only, referenced by a Cell object. */
class BaseCell {
    /** The production of the base cell. */
    static constexpr auto BASE_PRODUCTION = 0;
    /** The passability of the base cell. */
    static constexpr auto BASE_PASSABLE = true;
    /** The energy factor of the base cell. */
    static constexpr auto BASE_ENERGY_FACTOR = 0;

public:
    // TODO: possible usage of constant size array
    /** Map from player ID to player possessed entity here, if there is one. */
    std::unordered_map<Player::id_type, std::shared_ptr<Entity>> entities;

    /**
     * Add an entity by player, possibly merging with an existing entity.
     * @param player The player for the entity.
     * @param entity The entity to add.
     */
    void add_entity(const Player &player, std::shared_ptr<Entity> &entity);

    /**
     * Remove an entity by player.
     * @param player The player of the entity.
     */
    void remove_entity(const Player &player);

    /** Get the production of this cell. */
    virtual energy_type production() const { return BASE_PRODUCTION; }

    /** Get whether this cell is passable by an entity. */
    virtual bool is_passable() const { return BASE_PASSABLE; }

    /** Get the energy augmenting/dampening factor for the cell. */
    virtual long energy_factor() const { return BASE_ENERGY_FACTOR; }

    /**
     * JSON encoding function, dispatched on cell subtypes.
     * @param[out] json The JSON output.
     */
    virtual void to_json(nlohmann::json &json) const = 0;

    /** Convert the Cell to bot serial format. Used by base class operator<<. */
    virtual std::string to_bot_serial() const = 0;

    virtual ~BaseCell() = default;
};

/** A cell on the grid with production. */
class ProductionCell : public BaseCell {
    /** The production of the cell. */
    const energy_type _production;
public:
    energy_type production() const override { return _production; }

    /**
     * Create ProductionCell from production amount.
     * @param production The production amount.
     */
    explicit ProductionCell(energy_type production) : _production(production) {}

    /**
     * Create ProductionCell from JSON.
     * @param json The JSON.
     */
    explicit ProductionCell(const nlohmann::json &json);

    ~ProductionCell() override = default;
};

class NormalCell : public ProductionCell {
public:
    /** The name of the normal cell. */
    static constexpr auto CELL_TYPE_NAME = "n";

    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    /**
     * Create NormalCell from production amount.
     * @param production The production amount.
     */
    explicit NormalCell(energy_type production) : ProductionCell(production) {}

    /**
     * Create NormalCell from JSON.
     * @param json The JSON.
     */
    explicit NormalCell(const nlohmann::json &json) : ProductionCell(json) {}
};

/** An obstacle cell, with production but not passable. */
class ObstacleCell : public ProductionCell {
public:
    /** The name of the obstacle cell. */
    static constexpr auto CELL_TYPE_NAME = "o";

    bool is_passable() const override { return false; };

    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    /**
     * Create ObstacleCell from production amount.
     * @param production The production amount.
     */
    explicit ObstacleCell(energy_type production) : ProductionCell(production) {}

    /**
     * Create ObstacleCell from JSON.
     * @param json The JSON.
     */
    explicit ObstacleCell(const nlohmann::json &json) : ProductionCell(json) {}
};

/** A cell with an augmenting/diminishing energy factor. */
class EnergyFactorCell : public ProductionCell {
    /** The energy factor of the cell. */
    const long _energy_factor;
public:
    /** The name of the energy factor cell. */
    static constexpr auto CELL_TYPE_NAME = "e";

    long energy_factor() const override { return _energy_factor; };

    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    /**
     * Create EnergyFactorCell from production amount and energy factor.
     * @param production The production amount.
     * @param energy_factor The energy factor.
     */
    EnergyFactorCell(energy_type production, long energy_factor) :
            ProductionCell(production), _energy_factor(energy_factor) {}

    /**
     * Create EnergyFactorCell from JSON.
     * @param json The JSON.
     */
    explicit EnergyFactorCell(const nlohmann::json &json);
};

/** A factory cell that has no production. */
class FactoryCell : public BaseCell {
public:
    /** The name of the factory cell. */
    static constexpr auto CELL_TYPE_NAME = "f";

    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    /**
     * Create FactoryCell with no parameters.
     */
    FactoryCell() = default;

    /**
     * Create FactoryCell from JSON.
     * @param json The JSON.
     */
    explicit FactoryCell(const nlohmann::json &json) { (void) json; }
};

}

#endif // CELL_HPP
