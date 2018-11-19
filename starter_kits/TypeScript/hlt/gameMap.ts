import { Entity } from "./Entity";
import { Ship } from "./Ship";
import { Direction } from "./Direction";
import { Position } from "./Position";
import { Constants } from "./Constants";
import { MapCell } from "./MapCell";


/**
 * The game map.
 *
 * Can be indexed by a position, or by a contained entity.
 * Coordinates start at 0. Coordinates are normalized for you.
 */
export interface Location {
    x: number;
    y: number;
}
export class GameMap {
    constructor(
        private cells: MapCell[][], 
        public width: number, 
        public height: number
    ) {
    }

    /**
     * Getter for position object or entity objects within the game map
     * @param location the position or entity to access in this map
     * @returns the contents housing that cell or entity
    */
    get(location: Location | Position | Entity): MapCell {
        if (location instanceof Position) {
            location = this.normalize(location);
            return this.cells[location.y][location.x];
        } else if ((<Location>location).x !== undefined && (<Location>location).y !== undefined) {
            return this.get(new Position((<Location>location).x, (<Location>location).y));
        } else { 
            return this.get((<Entity>location).position);
        }
    }

    /**
     * Compute the Manhattan distance between two locations.
     * Accounts for wrap-around.
     * @param source The source from where to calculate
     * @param target The target to where calculate
     * @returns The distance between these items
    */
    calculateDistance(source: Position, target: Position) {
        source = this.normalize(source);
        target = this.normalize(target);
        const delta = source.sub(target).abs();
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
    normalize(position: Position) {
        let x = ((position.x % this.width) + this.width) % this.width;
        let y = ((position.y % this.height) + this.height) % this.height;
        return new Position(x, y);
    }

    /**
     * Determine the relative direction of the target compared to the
     * source (i.e. is the target north, south, east, or west of the
     * source). Does not account for wraparound.
     * @param source The source position
     * @param target The target position
     * @returns {[Direction | null, Direction | null]} A 2-tuple whose
     * elements are: the relative direction for each of the Y and X
     * coordinates (note the inversion), or null if the coordinates
     * are the same.
     */
    static _getTargetDirection(source: Position, target: Position): [Direction | null, Direction | null] {
        return [
            target.y > source.y ? Direction.South :
                (target.y < source.y ? Direction.North : null),
            target.x > source.x ? Direction.East :
                (target.x < source.x ? Direction.West : null),
        ];
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
    getUnsafeMoves(source: Position, destination: Position) {
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
    naiveNavigate(ship: Ship, destination: Position) {
        // No need to normalize destination since getUnsafeMoves does
        // that
        for (const direction of this.getUnsafeMoves(ship.position, destination)) {
            const targetPos = ship.position.directionalOffset(direction);

            if (!this.get(targetPos).isOccupied && (ship.haliteAmount >= (this.get(ship.position).haliteAmount / Constants.MOVE_COST_RATIO))) {
                this.get(targetPos).markUnsafe(ship);
                return direction;
            }
        }

        return Direction.Still;
    }

    static async _generate(getLine: () => Promise<string>) {
        const [mapWidth, mapHeight] = (await getLine())
            .split(/\s+/)
            .map(x => parseInt(x, 10));
        const gameMap = [];
        for (let y = 0; y < mapHeight; y++) {
            const cells = (await getLine()).split(/\s+/);
            const row = [];
            for (let x = 0; x < mapWidth; x++) {
                row.push(new MapCell(new Position(x, y), parseInt(cells[x], 10)));
            }
            gameMap.push(row);
        }

        return new GameMap(gameMap, mapWidth, mapHeight);
    }

    /**
     * Update this map object from the input given by the game
     * engine.
     */
    async _update(getLine: () => Promise<string>) {
        // Mark cells as safe for navigation (re-mark unsafe cells
        // later)
        for (let y = 0; y < this.height; y++) {
            for (let x = 0; x < this.width; x++) {
                this.get({x, y}).markSafe();
            }
        }

        const numChangedCells = parseInt(await getLine(), 10);
        for (let i = 0; i < numChangedCells; i++) {
            const line = (await getLine());
            const [x, y, cellEnergy] = line
                .split(/\s+/)
                .map(i => parseInt(i, 10));
            this.get({x, y}).haliteAmount = cellEnergy;
        }
    }
}
