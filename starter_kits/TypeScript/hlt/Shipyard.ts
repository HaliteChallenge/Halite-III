import { Commands } from "./commands";
import { Entity } from "./entity";

/** Represents a shipyard. */
export class Shipyard extends Entity {
    /** Return a move to spawn a new ship at your shipyard. */
    public spawn() {
        return Commands.GENERATE;
    }
}
