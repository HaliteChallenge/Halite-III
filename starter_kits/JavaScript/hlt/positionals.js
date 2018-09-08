const commands = require('./commands');

class Direction {
    constructor(dx, dy) {
        this.dx = dx;
        this.dy = dy;
    }

    equals(other) {
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
            return commands.NORTH;
        }
        else if (this.equals(Direction.South)) {
            return commands.SOUTH;
        }
        else if (this.equals(Direction.East)) {
            return commands.EAST;
        }
        else if (this.equals(Direction.West)) {
            return commands.WEST;
        }
        else if (this.equals(Direction.Still)) {
            return commands.STAY_STILL;
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
Direction.North = new Direction(0, -1);
Direction.South = new Direction(0, 1);
Direction.East = new Direction(1, 0);
Direction.West = new Direction(-1, 0);
Direction.Still = new Direction(0, 0);

class Position {
    constructor(x, y) {
        this.x = x;
        this.y = y;
    }

    directionalOffset(direction) {
        return this.add(new Position(direction.dx, direction.dy));
    }

    getSurroundingCardinals() {
        return Direction.getAllCardinals()
            .map(currentDirection => this.directionalOffset(currentDirection));
    }

    add(other) {
        return new Position(this.x + other.x, this.y + other.y);
    }

    sub(other) {
        return new Position(this.x - other.x, this.y - other.y);
    }

    addMut(other) {
        this.x += other.x;
        this.y += other.y;
    }

    subMut(other) {
        this.x -= other.x;
        this.y -= other.y;
    }

    abs() {
        return new Position(Math.abs(this.x), Math.abs(this.y));
    }

    equals(other) {
        return this.x === other.x && this.y === other.y;
    }

    toString() {
        return `${this.constructor.name}(${this.x}, ${this.y})`;
    }
}

module.exports = {
    Direction,
    Position,
};
