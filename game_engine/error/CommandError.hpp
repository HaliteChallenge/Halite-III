#ifndef COMMANDERROR_HPP
#define COMMANDERROR_HPP

#include <deque>
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

/** The type of a command context. */
using ErrorContext = std::deque<std::reference_wrapper<const Command>>;

/** Error for when bot commands are logically invalid. */
class BaseCommandError : public BotError {
protected:
    std::string buffer;           /** The message buffer of the error. */
    const ErrorContext _context;  /**< The command context. */

public:
    const Player::id_type player; /**< The player issuing the command. */
    bool ignored{};               /**< If true, this error was ignored by the engine. */

    /**
     * Construct BaseCommandError from player.
     * @param player The player.
     * @param context The command context.
     * @param ignored Whether this error was ignored by the engine.
     */
    BaseCommandError(Player::id_type player, ErrorContext context, bool ignored) :
            _context(std::move(context)), player(player), ignored(ignored) {}

    /**
     * Get the command that caused the error.
     * @return The command.
     */
    virtual const Command &command() const = 0;

    /**
     * Get the context commands of the error, i.e. the other commands that contributed to the issue.
     * @return The list of context commands.
     */
    virtual const ErrorContext &context() const { return _context; }

    /**
     * Get a message for the player log.
     * @return The message.
     */
    virtual std::string log_message() const = 0;

    /**
     * Get a message printed immediately before the context if there is any.
     * @return The message.
     */
    virtual std::string context_message() const {
        return "Commands contributing to this error:";
    }

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
private:
    const Command &_command;      /**< The command that caused the error. */

public:
    const Entity::id_type entity; /**< The entity. */

    /**
     * Get the command that caused the error.
     * @return The command.
     */
    const Command &command() const override { return _command; }

    /**
     * Get a message printed immediately before the context if there is any.
     * @return The message.
     */
    std::string context_message() const override {
        return "The other commands issued to this entity:";
    }

    /**
     * Get a message for the player log.
     * @return The message.
     */
    std::string log_message() const override;

    /**
     * Construct ExcessiveCommandsError from player, entity, and commands.
     * @param player The player.
     * @param command The first faulty command.
     * @param context The command context.
     * @param entity The entity.
     * @param ignored Whether this error was ignored by the engine.
     */
    ExcessiveCommandsError(Player::id_type player, const Command &command, ErrorContext context,
                           Entity::id_type entity, bool ignored = false) :
            BaseCommandError(player, std::move(context), ignored), _command(command), entity(entity) {
        buffer = log_message();
    }
};

/** Error for when too many spawn commands are attempted on one turn. */
class ExcessiveSpawnsError : public BaseCommandError {
private:
    const Command &_command;      /**< The command that caused the error. */

public:
    /**
     * Get the command that caused the error.
     * @return The command.
     */
    const Command &command() const override { return _command; }

    /**
     * Get a message printed immediately before the context if there is any.
     * @return The message.
     */
    std::string context_message() const override {
        return "The other spawn commands issued on this turn:";
    }

    /**
     * Get a message for the player log.
     * @return The message.
     */
    std::string log_message() const override;

    /**
     * Construct ExcessiveSpawnsError from player.
     * @param player The player.
     * @param command The first faulty command.
     * @param context The command context.
     * @param ignored Whether this error was ignored by the engine.
     */
    ExcessiveSpawnsError(Player::id_type player, const Command &command,
                         ErrorContext context, bool ignored = false)
            : BaseCommandError(player, std::move(context), ignored), _command(command) {
        buffer = log_message();
    }
};

/**
 * Error for when a required entity is not found.
 * @tparam Command The type of the command.
 */
template<class Command>
class EntityNotFoundError : public BaseCommandError {
private:
    const Command &_command;      /**< The command that caused the error. */

public:
    /**
     * Get the command that caused the error.
     * @return The command.
     */
    const Command &command() const override { return _command; }

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
            BaseCommandError(player, {}, ignored), _command(command) {
        buffer = log_message();
    }
};

/**
 * Error for when entity energy is insufficient.
 * @tparam Command The type of the command, or Player.
 */
template<class Command>
class InsufficientEnergyError : public BaseCommandError {
private:
    const Command &_command;      /**< The command that caused the error. */

public:
    const energy_type available; /**< The available energy. */
    const energy_type requested; /**< The requested energy. */

    /**
     * Get the command that caused the error.
     * @return The command.
     */
    const Command &command() const override { return _command; }

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
            BaseCommandError(player, {}, ignored), _command(command), available(available), requested(requested) {
        buffer = log_message();
    }
};

/** Error for when player energy is insufficient. */
class PlayerInsufficientEnergyError : public BaseCommandError {
private:
    const Command &_command;      /**< The command that caused the error. */

public:
    const energy_type available; /**< The available energy. */
    const energy_type requested; /**< The requested energy. */

    /**
     * Get the command that caused the error.
     * @return The command.
     */
    const Command &command() const override { return _command; }

    /**
     * Get a message for the player log.
     * @return The message.
     */
    std::string log_message() const override;

    /**
     * Get a message printed immediately before the context if there is any.
     * @return The message.
     */
    std::string context_message() const override {
        return "The other commands that used player energy on this turn:";
    }

    /**
     * Construct PlayerInsufficientEnergyError from player and available and requested energy.
     * @param player The player.
     * @param command The first faulty command.
     * @param context The command context.
     * @param available The available energy.
     * @param requested The requested energy.
     * @param ignored Whether this error was ignored by the engine.
     */
    PlayerInsufficientEnergyError(Player::id_type player, const Command &command, ErrorContext context,
                                  energy_type available, energy_type requested, bool ignored = false) :
            BaseCommandError(player, std::move(context), ignored), _command(command),
            available(available), requested(requested) {
        buffer = log_message();
    }
};

/**
 * Error for when a required cell is already owned.
 * @tparam Command The type of the command.
 */
template<class Command>
class CellOwnedError : public BaseCommandError {
private:
    const Command &_command;      /**< The command that caused the error. */

public:
    const Location cell;         /**< The location of the cell. */
    const Player::id_type owner; /**< The current owner of the cell. */

    /**
     * Get the command that caused the error.
     * @return The command.
     */
    const Command &command() const override { return _command; }

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
            BaseCommandError(player, {}, ignored), _command(command), cell(cell), owner(owner) {
        buffer = log_message();
    }
};

/**
 * Error for when a player causes their own entities to collide.
 * @tparam Command The type of the command.
 */
template<class Command>
class SelfCollisionError : public BaseCommandError {
private:
    const Command &_command;                     /** The command that caused the error. */

public:
    const Location cell;                         /**< The location of the collision. */
    const std::vector<Entity::id_type> entities; /**< The entities that collided. */

    /**
     * Get the command that caused the error.
     * @return The command.
     */
    const Command &command() const override { return _command; }


    /**
     * Get a message for the player log.
     * @return The message.
     */
    std::string log_message() const override;

    /**
     * Get a message printed immediately before the context if there is any.
     * @return The message.
     */
    std::string context_message() const override {
        return "Other commands that placed an entity on this cell:";
    }

    /**
     * Construct SelfCollisionError from player, command, context, cell, and entities.
     * @param player The player.
     * @param command The command.
     * @param context The context.
     * @param cell The cell of the collision.
     * @param entities The entities that collided.
     * @param ignored Whether this error was ignored by the engine.
     */
    SelfCollisionError(Player::id_type player, const Command &command, ErrorContext context, Location cell,
                       std::vector<Entity::id_type> entities, bool ignored = false) :
            BaseCommandError(player, std::move(context), ignored), _command(command), cell(cell),
            entities(std::move(entities)) {}
};

}

#endif // COMMANDERROR_HPP
