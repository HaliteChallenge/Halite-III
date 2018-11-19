import { Ship } from "./Ship";
import { Position } from "./Position";

/** A cell on the game map. */
export class MapCell {
    ship: Ship | null = null;
    structure: any = null;
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
        return this.ship !== null;
    }

    /**
     * Whether this cell has any structures.
     */
    get hasStructure() {
        return this.structure !== null;
    }

    /**
     * @returns The type of the structure in this cell, or null.
     */
    get structureType() {
        if (this.structure !== null) {
            return this.structure.constructor;
        }
        return null;
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