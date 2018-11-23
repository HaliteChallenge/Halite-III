import { Position } from "./Position";

/** Base entity class for Ships, Dropoffs, and Shipyards. */
export class Entity {
    constructor(public owner: number, public id: number, public position: Position) {
    }

    public toString() {
        return `${this.constructor.name}(id=${this.id}, ${this.position})`;
    }
}
