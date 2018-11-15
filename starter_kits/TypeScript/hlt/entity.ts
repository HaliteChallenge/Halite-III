import { Commands } from './commands';
import { Position, Direction } from "./positionals";
import { Constants } from './constants';

/** Base entity class for Ships, Dropoffs, and Shipyards. */
export class Entity {
    constructor(public owner: any, public id: number, public position: Position) {
        this.owner = owner;
        this.id = id;
        this.position = position;
    }

    toString() {
        return `${this.constructor.name}(id=${this.id}, ${this.position})`;
    }
}

/** Represents a dropoff. */
export class Dropoff extends Entity {
    /**
     * Create a Dropoff for a specific player from the engine input.
     */
    static async _generate(playerId: number, getLine: () => Promise<string>) {
        const [id, xPos, yPos] = (await getLine())
            .split(/\s+/)
            .map(x => parseInt(x, 10));
        return {id, dropoff: new Dropoff(playerId, id, new Position(xPos, yPos))};
    }
}

/** Represents a shipyard. */
export class Shipyard extends Entity {
    /** Return a move to spawn a new ship at your shipyard. */
    spawn() {
        return Commands.GENERATE;
    }
}

/** Represents a ship. */
export class Ship extends Entity {
    constructor(owner: any, id: number, position: Position, public haliteAmount: number) {
        super(owner, id, position);
        this.haliteAmount = haliteAmount;
    }

    /** Is this ship at max halite capacity? */
    get isFull() {
        return this.haliteAmount >= Constants.MAX_ENERGY;
    }

    /** Return a move to turn this ship into a dropoff. */
    makeDropoff() {
        return `${Commands.CONSTRUCT} ${this.id}`;
    }

    /**
     * Return a command to move this ship in a direction without
     * checking for collisions.
     */
    move(direction: string | Direction) {
        if (direction instanceof Direction) {
            direction = direction.toWireFormat();
        }
        return `${Commands.MOVE} ${this.id} ${direction}`;
    }

    /**
     * Return a command to not move this ship.
     *
     * Not strictly needed, since ships do nothing by default.
     */
    stayStill() {
        return `${Commands.MOVE} ${this.id} ${Commands.STAY_STILL}`;
    }

    /**
     * Create a Ship instance for a player using the engine input.
     * @param playerId the owner
     * @return The ship ID and ship object.
     * @private
     */
    static async _generate(playerId: number, getLine: () => Promise<string>) {
        const [shipId, xPos, yPos, halite] = (await getLine())
            .split(/\s+/)
            .map(x => parseInt(x, 10));
        return {shipId, ship: new Ship(playerId, shipId, new Position(xPos, yPos), halite)};
    }

    toString() {
        return `${this.constructor.name}(id=${this.id}, ${this.position}, cargo=${this.haliteAmount} halite)`;
    }
}
