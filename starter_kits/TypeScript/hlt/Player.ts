import { Ship } from "./Ship";
import { Shipyard } from "./Shipyard";
import { Dropoff } from "./Dropoff";

/** Player object, containing all entities/metadata for the player. */
export class Player {
    ships = new Map<number, Ship>();
    lostShips = new Map<number, Ship>();
    dropoffs = new Map<number, Dropoff>();
    lostDropoffs = new Map<number, Dropoff>();
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
     * Remove ship from the ships list and add it to the lostShips list
     * to keep the ship state even when lost. 
     */
    loseShip(shipId: number) {
        this.lostShips.set(shipId, <Ship>this.ships.get(shipId));
        this.ships.delete(shipId);
    }

    addShip(ship: Ship) {
        this.ships.set(ship.id, ship);
    }
}
