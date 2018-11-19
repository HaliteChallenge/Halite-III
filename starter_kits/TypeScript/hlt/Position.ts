import { Direction } from './Direction';

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