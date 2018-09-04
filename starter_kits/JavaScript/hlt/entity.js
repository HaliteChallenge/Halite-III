const { Direction, Position } = require('./positionals');

const commands = require('./commands');
const constants = require('./constants');

/** Base entity class for Ships, Dropoffs, and Shipyards. */
class Entity {
    constructor(owner, id, position) {
        this.owner = owner;
        this.id = id;
        this.position = position;
    }

    toString() {
        return `${this.constructor.name}(id=${this.id}, ${this.position})`;
    }
}

/** Represents a dropoff. */
class Dropoff extends Entity {
    /**
     * Create a Dropoff for a specific player from the engine input.
     * @private
     * @param playerId the player that owns this dropoff
     * @param {Function} getLine function to read a line of input
     * @returns {Dropoff}
     */
    static async _generate(playerId, getLine) {
        const [ id, xPos, yPos ] = (await getLine())
              .split(/\s+/)
              .map(x => parseInt(x, 10));
        return [ id, new Dropoff(playerId, id, new Position(xPos, yPos)) ];
    }
}

/** Represents a shipyard. */
class Shipyard extends Entity {
    /** Return a move to spawn a new ship at your shipyard. */
    spawn() {
        return commands.GENERATE;
    }
}

/** Represents a ship. */
class Ship extends Entity {
    constructor(owner, id, position, haliteAmount) {
        super(owner, id, position);
        this.haliteAmount = haliteAmount;
    }

    /** Is this ship at max halite capacity? */
    get isFull() {
        return this.haliteAmount >= constants.MAX_HALITE;
    }

    /** Return a move to turn this ship into a dropoff. */
    makeDropoff() {
        return `${commands.CONSTRUCT} ${this.id}`;
    }

    /**
     * Return a command to move this ship in a direction without
     * checking for collisions.
     * @param {String|Direction} direction the direction to move in
     * @returns {String} the command
     */
    move(direction) {
        if (direction.toWireFormat) {
            direction = direction.toWireFormat();
        }
        return `${commands.MOVE} ${this.id} ${direction}`;
    }

    /**
     * Return a command to not move this ship.
     *
     * Not strictly needed, since ships do nothing by default.
     */
    stayStill() {
        return `${commands.MOVE} ${this.id} ${commands.STAY_STILL}`;
    }

    /**
     * Create a Ship instance for a player using the engine input.
     * @param playerId the owner
     * @return The ship ID and ship object.
     * @private
     */
    static async _generate(playerId, getLine) {
        const [ shipId, xPos, yPos, halite ] = (await getLine())
              .split(/\s+/)
              .map(x => parseInt(x, 10));
        return [ shipId, new Ship(playerId, shipId, new Position(xPos, yPos), halite) ];
    }

    toString() {
        return `${this.constructor.name}(id=${this.id}, ${this.position}, cargo=${this.haliteAmount} halite)`;
    }
}

module.exports = {
    Entity,
    Ship,
    Dropoff,
    Shipyard,
};
