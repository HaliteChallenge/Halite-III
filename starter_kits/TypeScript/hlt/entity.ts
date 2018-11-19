import { Position } from "./Position";

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
