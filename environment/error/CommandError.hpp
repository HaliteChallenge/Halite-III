#ifndef COMMANDERROR_HPP
#define COMMANDERROR_HPP

#include <stdexcept>
#include <string>

#include "BotError.hpp"
#include "Command.hpp"
#include "Player.hpp"

namespace hlt {

class Command;

class BaseCommandError;

/** Type of command errors exposed to users. */
using CommandError = std::unique_ptr<BaseCommandError>;

/** Error for when bot commands are logically invalid. */
class BaseCommandError : public BotError {
protected:
    /** The message buffer of the error. */
    std::string buffer;

public:
    const Player::id_type player; /**< The player issuing the command. */
    bool ignored{};               /**< If true, this error was ignored by the engine. */

    /**
     * Construct BaseCommandError from player.
     * @param player The player.
     * @param ignored Whether this error was ignored by the engine.
     */
    BaseCommandError(Player::id_type player, bool ignored) : player(player), ignored(ignored) {}

    /**
     * Get a message for the player log.
     * @return The message.
     */
    virtual std::string log_message() const = 0;

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        return buffer.c_str();
    }
};

/** Error for when too many commands are issued to an entity. */
class ExcessiveCommandsError : public BaseCommandError {
public:
    const Entity::id_type entity; /**< The entity. */
    const int num_commands;       /**< The total number of commands issued. */

    /**
     * Get a message for the player log.
     * @return The message.
     */
    std::string log_message() const override;

    /**
     * Construct ExcessiveCommandsError from player, entity, and number of commands.
     * @param player The player.
     * @param entity The entity.
     * @param num_commands The number of commands.
     * @param ignored Whether this error was ignored by the engine.
     */
    ExcessiveCommandsError(Player::id_type player, Entity::id_type entity, int num_commands, bool ignored = false) :
            BaseCommandError(player, ignored), entity(entity), num_commands(num_commands) {
        buffer = log_message();
    }
};

/** Error for when too many spawn commands are attempted on one turn. */
class ExcessiveSpawnsError : public BaseCommandError {
public:
    /**
     * Get a message for the player log.
     * @return The message.
     */
    std::string log_message() const override;

    /**
     * Construct ExcessiveSpawnsError from player.
     * @param player The player.
     * @param ignored Whether this error was ignored by the engine.
     */
    explicit ExcessiveSpawnsError(Player::id_type player, bool ignored = false) : BaseCommandError(player, ignored) {
        buffer = log_message();
    }
};

/**
 * Error for when a required entity is not found.
 * @tparam Command The type of the command.
 */
template<class Command>
class EntityNotFoundError : public BaseCommandError {
public:
    const Command command; /**< The command. */

    /**
     * Get a message for the player log.
     * @return The message.
     */
    std::string log_message() const override;

    /**
     * Construct EntityNotFoundError from player and command.
     * @param player The player.
     * @param command The command.
     * @param ignored Whether this error was ignored by the engine.
     */
    EntityNotFoundError(Player::id_type player, const Command &command, bool ignored = false) :
            BaseCommandError(player, ignored), command(command) {
        buffer = log_message();
    }
};

/**
 * Error for when entity energy is insufficient.
 * @tparam Command The type of the command, or Player.
 */
template<class Command>
class InsufficientEnergyError : public BaseCommandError {
public:
    const Command command;       /**< The command. */
    const energy_type available; /**< The available energy. */
    const energy_type requested; /**< The requested energy. */

    /**
     * Get a message for the player log.
     * @return The message.
     */
    std::string log_message() const override;

    /**
     * Construct InsufficientEnergyError from player, command, available and requested energy.
     * @param player The player.
     * @param command The command.
     * @param available The available energy.
     * @param requested The requested energy.
     * @param ignored Whether this error was ignored by the engine.
     */
    InsufficientEnergyError(Player::id_type player, const Command &command,
                            energy_type available, energy_type requested, bool ignored = false) :
            BaseCommandError(player, ignored), command(command), available(available), requested(requested) {
        buffer = log_message();
    }
};

/** Error for when player energy is insufficient. */
class PlayerInsufficientEnergyError : public BaseCommandError {
public:
    const energy_type available; /**< The available energy. */
    const energy_type requested; /**< The requested energy. */

    /**
     * Get a message for the player log.
     * @return The message.
     */
    std::string log_message() const override;

    /**
     * Construct PlayerInsufficientEnergyError from player and available and requested energy.
     * @param player The player.
     * @param available The available energy.
     * @param requested The requested energy.
     * @param ignored Whether this error was ignored by the engine.
     */
    PlayerInsufficientEnergyError(Player::id_type player, energy_type available,
                                  energy_type requested, bool ignored = false) :
            BaseCommandError(player, ignored), available(available), requested(requested) {
        buffer = log_message();
    }
};

/**
 * Error for when a required cell is already owned.
 * @tparam Command The type of the command.
 */
template<class Command>
class CellOwnedError : public BaseCommandError {
public:
    const Command command;       /**< The command. */
    const Location cell;         /**< The location of the cell. */
    const Player::id_type owner; /**< The current owner of the cell. */

    /**
     * Get a message for the player log.
     * @return The message.
     */
    std::string log_message() const override;

    /**
     * Construct CellOwnedError from player, command, cell, and current owner.
     * @param player The player.
     * @param command The command.
     * @param cell The cell.
     * @param owner The current owner.
     * @param ignored Whether this error was ignored by the engine.
     */
    CellOwnedError(Player::id_type player, const Command &command, Location cell,
                   Player::id_type owner, bool ignored = false) :
            BaseCommandError(player, ignored), command(command), cell(cell), owner(owner) {
        buffer = log_message();
    }
};

}

#endif // COMMANDERROR_HPP
