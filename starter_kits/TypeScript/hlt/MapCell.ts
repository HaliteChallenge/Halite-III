import { Dropoff } from "./Dropoff";
import { Position } from "./Position";
import { Ship } from "./Ship";
import { Shipyard } from "./Shipyard";

/** A cell on the game map. */
export class MapCell {
    public ship?: Ship;
    public structure?: Shipyard | Dropoff;

    constructor(public position: Position, public haliteAmount: number) {
    }

    /**
     * Whether this cell has no ships or structures.
     */
    get isEmpty() {
        return !this.isOccupied && !this.hasStructure;
    }

    /**
     * Whether this cell has any ships.
     */
    get isOccupied() {
        return this.ship !== undefined;
    }

    /**
     * Whether this cell has any structures.
     */
    get hasStructure() {
        return this.structure !== undefined;
    }

    /**
     * Mark this cell as unsafe (occupied) for navigation.
     */
    public markUnsafe(ship: Ship) {
        this.ship = ship;
    }

    public markSafe() {
        this.ship = undefined;
    }

    public equals(other: MapCell) {
        return this.position.equals(other.position);
    }

    public toString() {
        return `MapCell(${this.position}, halite=${this.haliteAmount})`;
    }
}
