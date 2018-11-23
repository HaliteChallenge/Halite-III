import { Dropoff } from "./Dropoff";
import { Ship } from "./Ship";
import { Shipyard } from "./Shipyard";

/** Player object, containing all entities/metadata for the player. */
export class Player {
    public ships = new Map<number, Ship>();
    public lostShips = new Map<number, Ship>();
    public dropoffs = new Map<number, Dropoff>();
    public lostDropoffs = new Map<number, Dropoff>();
    constructor(public id: number, public shipyard: Shipyard, public haliteAmount = 0) {
    }

    /** Get a single ship by its ID. */
    public getShip(shipId: number) {
        return this.ships.get(shipId);
    }

    /** Get a list of the player's ships. */
    public getShips() {
        return Array.from(this.ships.values());
    }

    /** Get a single dropoff by its ID. */
    public getDropoff(dropoffId: number) {
        return this.dropoffs.get(dropoffId);
    }

    /** Get a list of the player's dropoffs. */
    public getDropoffs() {
        return Array.from(this.dropoffs.values());
    }

    /** Check whether a ship with a given ID exists. */
    public hasShip(shipId: number) {
        return this.ships.has(shipId);
    }

    /**
     * Remove ship from the ships list and add it to the lostShips list
     * to keep the ship state even when lost.
     */
    public loseShip(shipId: number) {
        this.lostShips.set(shipId,  this.ships.get(shipId) as Ship);
        this.ships.delete(shipId);
    }

    public addShip(ship: Ship) {
        this.ships.set(ship.id, ship);
    }
}
