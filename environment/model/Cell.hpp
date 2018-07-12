#ifndef CELL_HPP
#define CELL_HPP

#include <iostream>

#include "Entity.hpp"
#include "Player.hpp"

#include "nlohmann/json_fwd.hpp"

namespace hlt {

// TODO: removing the cell alias may not be such a great idea

/** Abstract cell data type. Subclasses are intended to be data-only, referenced by a Cell object. */
class Cell {
    /** The energy of the base cell. */
    static constexpr auto BASE_ENERGY = 0;

public:
    /** The names of cell types. */
    enum Name : char {
        Normal = 'n',
        Obstacle = 'o',
        EnergyFactor = 'e',
        Factory = 'f'
    };

    /** Entity on this Cell. */
    Entity::id_type entity = Entity::None;

    /** Get the energy on this cell. */
    virtual energy_type energy() const { return BASE_ENERGY; }

    /** Get whether this cell is passable by an entity. */
    virtual bool is_passable() const { return true; }

    /** Process effect of current entity mining the cell. */
    virtual void mine() {}

    /**
     * JSON encoding function, dispatched on cell subtypes.
     * @param[out] json The JSON output.
     */
    virtual void to_json(nlohmann::json &json) const = 0;

    /** Convert the Cell to bot serial format. Used by base class operator<<. */
    virtual std::string to_bot_serial() const = 0;

    /** Virtual destructor. */
    virtual ~Cell() = default;
};

/**
 * Convert a Cell to JSON format.
 * @param[out] json The output JSON.
 * @param cell The cell to convert.
 */
void to_json(nlohmann::json &json, const std::unique_ptr<Cell> &cell);

/**
 * Write a Cell to bot serial format.
 * @param ostream The output stream.
 * @param cell The cell to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Cell &cell);

class NormalCell : public Cell {
    /** The energy of the cell. */
    energy_type _energy;
public:
    energy_type energy() const override { return _energy; }

    void mine() override {
        if (_energy > 0) {
            // TODO: implement
        }
    }

    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    explicit NormalCell(energy_type energy) : _energy(energy) {}
};

/** An impassable obstacle cell. */
class ObstacleCell : public Cell {
public:
    bool is_passable() const override { return false; };

    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    ObstacleCell() = default;
};

/** A factory cell. */
class FactoryCell : public Cell {
public:
    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    /** Create FactoryCell with no parameters. */
    FactoryCell() = default;
};

class DropZoneCell : public Cell {
    Player::id_type owner;
public:
    void to_json(nlohmann::json &json) const override;

    std::string to_bot_serial() const override;

    DropZoneCell(Player::id_type owner) : owner(owner) {}
};

}

#endif // CELL_HPP
