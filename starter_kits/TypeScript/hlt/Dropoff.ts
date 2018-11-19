import { Position } from "./Position";
import { Entity } from './entity';

/** Represents a dropoff. */
export class Dropoff extends Entity {
    /**
     * Create a Dropoff for a specific player from the engine input.
     */
    static async _generate(playerId: number, getLine: () => Promise<string>) {
        const [id, xPos, yPos] = (await getLine())
            .split(/\s+/)
            .map(x => parseInt(x, 10));
        return { id, dropoff: new Dropoff(playerId, id, new Position(xPos, yPos)) };
    }
}