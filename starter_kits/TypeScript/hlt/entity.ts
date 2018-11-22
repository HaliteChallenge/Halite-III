import { Position } from "./Position";

/** Base entity class for Ships, Dropoffs, and Shipyards. */
export class Entity {
    constructor(public owner: number, public id: number, public position: Position) {
    }

    toString() {
        return `${this.constructor.name}(id=${this.id}, ${this.position})`;
    }
}
