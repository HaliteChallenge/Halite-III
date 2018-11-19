import { Ship } from "./Ship";
import { Shipyard } from "./Shipyard";
import { Dropoff } from "./Dropoff";
import { Position } from "./Position";

/** Player object, containing all entities/metadata for the player. */
export class Player {
    private ships = new Map<number, Ship>();
    private dropoffs = new Map<number, Dropoff>();
    constructor(public id: number, public shipyard: Shipyard, public haliteAmount = 0) {
    }

    /** Get a single ship by its ID. */
    getShip(shipId: number) {
        return this.ships.get(shipId);
    }

    /** Get a list of the player's ships. */
    getShips() {
        return Array.from(this.ships.values());
    }

    /** Get a single dropoff by its ID. */
    getDropoff(dropoffId: number) {
        return this.dropoffs.get(dropoffId);
    }

    /** Get a list of the player's dropoffs. */
    getDropoffs() {
        return Array.from(this.dropoffs.values());
    }

    /** Check whether a ship with a given ID exists. */
    hasShip(shipId: number) {
        return this.ships.has(shipId);
    }

    /**
     * Create a player object using input from the game engine.
     */
    static async _generate(getLine: () => Promise<string>) {
        const line = await getLine();
        const [playerId, shipyardX, shipyardY] = line
            .split(/\s+/)
            .map(x => parseInt(x, 10));
        return new Player(playerId, new Shipyard(playerId, -1, new Position(shipyardX, shipyardY)));
    }
    
    /**
     * Update the player object for the current turn using input from
     * the game engine.
     */
    async _update(numShips: number, numDropoffs: number, halite: number, getLine: () => Promise<string>) {
        this.haliteAmount = halite;
        this.ships = new Map();
        for (let i = 0; i < numShips; i++) {
            const { shipId, ship } = await Ship._generate(this.id, getLine);
            this.ships.set(shipId, ship);
        }
        this.dropoffs = new Map();
        for (let i = 0; i < numDropoffs; i++) {
            const { id, dropoff } = await Dropoff._generate(this.id, getLine);
            this.dropoffs.set(id, dropoff);
        }
    }
}