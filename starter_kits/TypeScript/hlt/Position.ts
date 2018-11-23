import { Direction } from "./Direction";

export class Position {
    constructor(public x: number, public y: number) {
        this.x = x;
        this.y = y;
    }

    public directionalOffset(direction: Direction) {
        return this.add(new Position(direction.dx, direction.dy));
    }

    public getSurroundingCardinals() {
        return Direction.getAllCardinals()
            .map((currentDirection) => this.directionalOffset(currentDirection));
    }

    public add(other: Position) {
        return new Position(this.x + other.x, this.y + other.y);
    }

    public sub(other: Position) {
        return new Position(this.x - other.x, this.y - other.y);
    }

    public addMut(other: Position) {
        this.x += other.x;
        this.y += other.y;
    }

    public subMut(other: Position) {
        this.x -= other.x;
        this.y -= other.y;
    }

    public abs() {
        return new Position(Math.abs(this.x), Math.abs(this.y));
    }

    public equals(other: Position) {
        return this.x === other.x && this.y === other.y;
    }

    public toString() {
        return `${this.constructor.name}(${this.x}, ${this.y})`;
    }
}
