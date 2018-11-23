import { Constants } from "./Constants";
import { Direction } from "./Direction";
import { Entity } from "./Entity";
import { MapCell } from "./MapCell";
import { Position } from "./Position";
import { Ship } from "./Ship";

/**
 * The game map.
 *
 * Can be indexed by a position, or by a contained entity.
 * Coordinates start at 0. Coordinates are normalized for you.
 */
export class GameMap {

    /**
     * Determine the relative direction of the target compared to the
     * source (i.e. is the target north, south, east, or west of the
     * source). Does not account for wraparound.
     * @returns A 2-tuple whose
     * elements are: the relative direction for each of the Y and X
     * coordinates (note the inversion), or undefined if the coordinates
     * are the same.
     */
    public static _getTargetDirection(
        source: Position, destination: Position,
    ): [Direction | undefined, Direction | undefined] {
        return [
            destination.y > source.y ? Direction.South :
                (destination.y < source.y ? Direction.North : undefined),
            destination.x > source.x ? Direction.East :
                (destination.x < source.x ? Direction.West : undefined),
        ];
    }
    public readonly cells: MapCell[][];
    public width = 0;
    public height = 0;

    constructor(cells: number[][]) {
        this.width = cells[0].length;
        this.height = cells.length;
        this.cells = cells.map((row, y) => row.map((halite, x) => new MapCell(new Position(x, y), halite)));
    }

    /**
     * Getter for position object or entity objects within the game map
     * @param location the position or entity to access in this map
     * @returns the contents housing that cell or entity
     */
    public get(positionOrEntity: Position | Entity): MapCell {
        if (positionOrEntity instanceof Position) {
            const normalizedPosition = this.normalize(positionOrEntity);
            return this.cells[normalizedPosition.y][normalizedPosition.x];
        } else {
            return this.get(positionOrEntity.position);
        }
    }

    /**
     * Compute the Manhattan distance between two locations.
     * Accounts for wrap-around.
     * @param source The source from where to calculate
     * @param destination The target to where calculate
     * @returns The distance between these items
     */
    public calculateDistance(source: Position, destination: Position) {
        source = this.normalize(source);
        destination = this.normalize(destination);
        const delta = source.sub(destination).abs();
        return Math.min(delta.x, this.width - delta.x) +
            Math.min(delta.y, this.height - delta.y);
    }

    /**
     * Normalized the position within the bounds of the toroidal map.
     * i.e.: Takes a point which may or may not be within width and
     * height bounds, and places it within those bounds considering
     * wraparound.
     * @param {Position} position A position object.
     * @returns A normalized position object fitting within the bounds of the map
     */
    public normalize(position: Position) {
        const x = ((position.x % this.width) + this.width) % this.width;
        const y = ((position.y % this.height) + this.height) % this.height;
        return new Position(x, y);
    }

    /**
     * Return a list of Direction(s) that move closer to the
     * destination, if any.
     *
     * This does not account for collisions. Multiple directions may
     * be returned if movement in both coordinates is viable.
     *
     * @param source The (normalized) source position
     * @param destination The (normalized) target position
     * @returns A list of Directions moving towards the target (if
     * any)
     */
    public getUnsafeMoves(source: Position, destination: Position) {
        source = this.normalize(source);
        destination = this.normalize(destination);

        const possibleMoves: Direction[] = [];
        const distance = destination.sub(source).abs();
        const [yDir, xDir] = GameMap._getTargetDirection(source, destination);

        // Account for wraparound
        if (xDir && distance.x !== 0) {
            possibleMoves.push(distance.x < (this.width / 2) ? xDir : xDir.invert());
        }
        if (yDir && distance.y !== 0) {
            possibleMoves.push(distance.y < (this.height / 2) ? yDir : yDir.invert());
        }

        return possibleMoves;
    }

    /**
     * Returns a singular safe move towards the destination. Marks the next cell as occupied.
     * @param ship - the ship to move
     * @param destination - the location to move towards
     * @return A direction towards the destination that is unoccupied.
     */
    public naiveNavigate(ship: Ship, destination: Position) {
        // No need to normalize destination since getUnsafeMoves does
        // that
        for (const direction of this.getUnsafeMoves(ship.position, destination)) {
            const targetPos = ship.position.directionalOffset(direction);

            if (!this.get(targetPos).isOccupied &&
                (ship.haliteAmount >= (this.get(ship.position).haliteAmount / Constants.MOVE_COST_RATIO))
            ) {
                this.get(targetPos).markUnsafe(ship);
                return direction;
            }
        }

        return Direction.Still;
    }

    public toString() {
        return JSON.stringify(this);
    }
}
