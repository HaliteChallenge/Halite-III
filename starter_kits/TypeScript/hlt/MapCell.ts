import { Ship } from "./Ship";
import { Position } from "./Position";
import { Shipyard } from "./Shipyard";
import { Dropoff } from "./Dropoff";

/** A cell on the game map. */
export class MapCell {
    ship: Ship | undefined = undefined;
    structure: Shipyard | Dropoff | undefined = undefined;
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
     *
     * Use in conjunction with {@link GameMap#getSafeMove}.
     */
    markUnsafe(ship: Ship) {
        this.ship = ship;
    }

    equals(other: MapCell) {
        return this.position.equals(other.position);
    }
    
    toString() {
        return `MapCell(${this.position}, halite=${this.haliteAmount})`;
    }
}