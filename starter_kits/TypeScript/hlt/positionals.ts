import { Commands } from './commands';

export class Direction {
    static North = new Direction(0, -1);
    static South = new Direction(0, 1);
    static East = new Direction(1, 0);
    static West = new Direction(-1, 0);
    static Still = new Direction(0, 0);

    constructor(public dx: number, public dy: number) {
        this.dx = dx;
        this.dy = dy;
    }

    equals(other: Direction) {
        return this.dx === other.dx && this.dy === other.dy;
    }

    toString() {
        return `${this.constructor.name}(${this.dx}, ${this.dy})`;
    }

    static getAllCardinals() {
        return [ Direction.North, Direction.South, Direction.East, Direction.West ];
    }

    toWireFormat() {
        if (this.equals(Direction.North)) {
            return Commands.NORTH;
        }
        else if (this.equals(Direction.South)) {
            return Commands.SOUTH;
        }
        else if (this.equals(Direction.East)) {
            return Commands.EAST;
        }
        else if (this.equals(Direction.West)) {
            return Commands.WEST;
        }
        else if (this.equals(Direction.Still)) {
            return Commands.STAY_STILL;
        }
        throw new Error(`Non-cardinal direction cannot be converted to wire format: ${this}`);
    }

    invert() {
        if (this.equals(Direction.North)) {
            return Direction.South;
        }
        else if (this.equals(Direction.South)) {
            return Direction.North;
        }
        else if (this.equals(Direction.East)) {
            return Direction.West;
        }
        else if (this.equals(Direction.West)) {
            return Direction.East;
        }
        else if (this.equals(Direction.Still)) {
            return Direction.Still;
        }
        throw new Error(`Non-cardinal direction cannot be inverted: ${this}`);
    }
}

export class Position {
    constructor(public x: number, public y: number) {
        this.x = x;
        this.y = y;
    }

    directionalOffset(direction: Direction) {
        return this.add(new Position(direction.dx, direction.dy));
    }

    getSurroundingCardinals() {
        return Direction.getAllCardinals()
            .map(currentDirection => this.directionalOffset(currentDirection));
    }

    add(other: Position) {
        return new Position(this.x + other.x, this.y + other.y);
    }

    sub(other: Position) {
        return new Position(this.x - other.x, this.y - other.y);
    }

    addMut(other: Position) {
        this.x += other.x;
        this.y += other.y;
    }

    subMut(other: Position) {
        this.x -= other.x;
        this.y -= other.y;
    }

    abs() {
        return new Position(Math.abs(this.x), Math.abs(this.y));
    }

    equals(other: Position) {
        return this.x === other.x && this.y === other.y;
    }

    toString() {
        return `${this.constructor.name}(${this.x}, ${this.y})`;
    }
}
