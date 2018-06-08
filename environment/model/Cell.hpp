#ifndef CELL_HPP
#define CELL_HPP

#include <iostream>

#include "Entity.hpp"

#include "util/json.hpp"

namespace hlt {

class BaseCell;

/** Container type for cells, exposed to outside users like Map. */
using Cell = std::shared_ptr<BaseCell>;

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
    static constexpr auto BASE_PRODUCTION = 0;
    static constexpr auto BASE_PASSABLE = true;
    static constexpr auto BASE_ENERGY_FACTOR = 0;

public:
    /** Get the production of this cell. */
    virtual long production() const { return BASE_PRODUCTION; }

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
};

/** A cell on the grid with production. */
class ProductionCell : public BaseCell {
    /** The production of the cell. */
    long _production;
public:
    long production() const override { return _production; }

    /**
     * Create ProductionCell from production amount.
     * @param production The production amount.
     */
    explicit ProductionCell(long production) : _production(production) {}

    /**
     * Create ProductionCell from JSON.
     * @param json The JSON.
     */
    explicit ProductionCell(const nlohmann::json &json);
};

class NormalCell : public ProductionCell {
public:
    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    /**
     * Create NormalCell from production amount.
     * @param production The production amount.
     */
    explicit NormalCell(long production) : ProductionCell(production) {}

    /**
     * Create NormalCell from JSON.
     * @param json The JSON.
     */
    explicit NormalCell(const nlohmann::json &json) : ProductionCell(json) {}
};

/** An obstacle cell, with production but not passable. */
class ObstacleCell : public ProductionCell {
public:
    bool is_passable() const override { return false; };

    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    /**
     * Create ObstacleCell from production amount.
     * @param production The production amount.
     */
    explicit ObstacleCell(long production) : ProductionCell(production) {}

    /**
     * Create ObstacleCell from JSON.
     * @param json The JSON.
     */
    explicit ObstacleCell(const nlohmann::json &json) : ProductionCell(json) {}
};

/** A cell with an augmenting/diminishing energy factor. */
class EnergyFactorCell : public ProductionCell {
    long _energy_factor;
public:
    long energy_factor() const override { return _energy_factor; };

    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    /**
     * Create EnergyFactorCell from production amount and energy factor.
     * @param production The production amount.
     * @param energy_factor The energy factor.
     */
    EnergyFactorCell(long production, long energy_factor) :
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
    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    /**
     * Create FactoryCell from JSON.
     * @param json The JSON.
     */
    explicit FactoryCell(const nlohmann::json &json) {}
};

}

#endif // CELL_HPP
