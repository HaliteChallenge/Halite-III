const constants = require('./constants');
const { Ship, Dropoff, Shipyard } = require('./entity');
const { Direction, Position } = require('./positionals');

/** Player object, containing all entities/metadata for the player. */
class Player {
    constructor(playerId, shipyard, halite=0) {
        this.id = playerId;
        this.shipyard = shipyard;
        this.haliteAmount = halite;
        this._ships = new Map();
        this._dropoffs = new Map();
    }

    /** Get a single ship by its ID. */
    getShip(shipId) {
        return this._ships.get(shipId);
    }

    /** Get a list of the player's ships. */
    getShips() {
        const result = [];
        for (const ship of this._ships.values()) {
            result.push(ship);
        }
        return result;
    }

    /** Get a single dropoff by its ID. */
    getDropoff(dropoffId) {
        return this._dropoffs.get(dropoffId);
    }

    /** Get a list of the player's dropoffs. */
    getDropoffs() {
        const result = [];
        for (const dropoff of this._dropoffs.values()) {
            result.push(dropoff);
        }
        return result;
    }

    /** Check whether a ship with a given ID exists. */
    hasShip(shipId) {
        return this._ships.has(shipId);
    }

    /**
     * Create a player object using input from the game engine.
     * @private
     */
    static async _generate(getLine) {
        const line = await getLine();
        const [ playerId, shipyardX, shipyardY ] = line
              .split(/\s+/)
              .map(x => parseInt(x, 10));
        return new Player(playerId,
                          new Shipyard(playerId, -1, new Position(shipyardX, shipyardY)));
    }

    /**
     * Update the player object for the current turn using input from
     * the game engine.
     * @private
     */
    async _update(numShips, numDropoffs, halite, getLine) {
        this.haliteAmount = halite;
        this._ships = new Map();
        for (let i = 0; i < numShips; i++) {
            const [ shipId, ship ] = await Ship._generate(this.id, getLine);
            this._ships.set(shipId, ship);
        }
        this._dropoffs = new Map();
        for (let i = 0; i < numDropoffs; i++) {
            const [ dropoffId, dropoff ] = await Dropoff._generate(this.id, getLine);
            this._dropoffs.set(dropoffId, dropoff);
        }
    }
}

/** A cell on the game map. */
class MapCell {
    constructor(position, halite) {
        this.position = position;
        this.haliteAmount = halite;
        this.ship = null;
        this.structure = null;
    }

    /**
     * @returns {Boolean} whether this cell has no ships or structures.
     */
    get isEmpty() {
        return !this.isOccupied && !this.hasStructure;
    }

    /**
     * @returns {Boolean} whether this cell has any ships.
     */
    get isOccupied() {
        return this.ship !== null;
    }

    /**
     * @returns {Boolean} whether this cell has any structures.
     */
    get hasStructure() {
        return this.structure !== null;
    }

    /**
     * @returns The type of the structure in this cell, or null.
     */
    get structureType() {
        if (this.hasStructure) {
            return this.structure.constructor;
        }
        return null;
    }

    /**
     * Mark this cell as unsafe (occupied) for navigation.
     *
     * Use in conjunction with {@link GameMap#getSafeMove}.
     *
     * @param {Ship} ship The ship occupying this cell.
     */
    markUnsafe(ship) {
        this.ship = ship;
    }

    equals(other) {
        return this.position.equals(other.position);
    }

    toString() {
        return `MapCell(${this.position}, halite=${this.haliteAmount})`;
    }
}

/**
 * The game map.
 *
 * Can be indexed by a position, or by a contained entity.
 * Coordinates start at 0. Coordinates are normalized for you.
 */
class GameMap {
    constructor(cells, width, height) {
        this.width = width;
        this.height = height;
        this._cells = cells;
    }

    /**
     * Getter for position object or entity objects within the game map
     * @param location the position or entity to access in this map
     * @returns the contents housing that cell or entity
    */
    get(...args) {
        if (args.length === 2) {
            return this._cells[args[1]][args[0]];
        }
        let [ location ] = args;
        if (location instanceof Position) {
            location = this.normalize(location);
            return this._cells[location.y][location.x];
        }
        else if (location.position) {
            return this.get(location.position);
        }
        return null;
    }

    /**
     * Compute the Manhattan distance between two locations.
     * Accounts for wrap-around.
     * @param source The source from where to calculate
     * @param target The target to where calculate
     * @returns The distance between these items
    */
    calculateDistance(source, target) {
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
    normalize(position) {
        let x = ((position.x % this.width) + this.width) % this.width;
        let y = ((position.y % this.height) + this.height) % this.height;
        return new Position(x, y);
    }

    /**
     * Determine the relative direction of the target compared to the
     * source (i.e. is the target north, south, east, or west of the
     * source). Does not account for wraparound.
     * @param {Position} source The source position
     * @param {Position} target The target position
     * @returns {[Direction | null, Direction | null]} A 2-tuple whose
     * elements are: the relative direction for each of the Y and X
     * coordinates (note the inversion), or null if the coordinates
     * are the same.
     */
    static _getTargetDirection(source, target) {
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
     * @param {Position} source The (normalized) source position
     * @param {Position} destination The (normalized) target position
     * @returns A list of Directions moving towards the target (if
     * any)
     */
    getUnsafeMoves(source, destination) {
        if (!(source instanceof Position && destination instanceof Position)) {
            throw new Error("source and destination must be of type Position");
        }

        source = this.normalize(source);
        destination = this.normalize(destination);

        const possibleMoves = [];
        const distance = destination.sub(source).abs();
        const [ yDir, xDir ] = GameMap._getTargetDirection(source, destination);

        if (distance.x !== 0) {
            possibleMoves.push(distance.x < (this.width / 2) ? xDir : xDir.invert());
        }
        if (distance.y !== 0) {
            possibleMoves.push(distance.y < (this.height / 2) ? yDir : yDir.invert());
        }

        return possibleMoves;
    }

    /**
     * Returns a singular safe move towards the destination.
     * @param {Ship} ship - the ship to move
     * @param {Position} destination - the location to move towards
     * @return {Direction}
     */
    naiveNavigate(ship, destination) {
        // No need to normalize destination since getUnsafeMoves does
        // that
        for (const direction of this.getUnsafeMoves(ship.position, destination)) {
            const targetPos = ship.position.directionalOffset(direction);

            if (!this.get(targetPos).isOccupied) {
                this.get(targetPos).markUnsafe(ship);
                return direction;
            }
        }

        return Direction.Still;
    }

    static async _generate(getLine) {
        const [ mapWidth, mapHeight ] = (await getLine())
              .split(/\s+/)
              .map(x => parseInt(x, 10));
        const gameMap = [];
        for (let i = 0; i < mapHeight; i++) {
            const row = [];
            row.fill(null, 0, mapWidth);
            gameMap.push(row);
        }

        for (let y = 0; y < mapHeight; y++) {
            const cells = (await getLine()).split(/\s+/);
            for (let x = 0; x < mapWidth; x++) {
                gameMap[y][x] = new MapCell(new Position(x, y), parseInt(cells[x], 10));
            }
        }

        return new GameMap(gameMap, mapWidth, mapHeight);
    }

    /**
     * Update this map object from the input given by the game
     * engine.
     */
    async _update(getLine) {
        // Mark cells as safe for navigation (re-mark unsafe cells
        // later)
        for (let y = 0; y < this.height; y++) {
            for (let x = 0; x < this.width; x++) {
                this.get(x, y).ship = null;
            }
        }

        const numChangedCells = parseInt(await getLine(), 10);
        for (let i = 0; i < numChangedCells; i++) {
            const line = (await getLine());
            const [ cellX, cellY, cellEnergy ] = line
                  .split(/\s+/)
                  .map(x => parseInt(x, 10));
            this.get(cellX, cellY).haliteAmount = cellEnergy;
        }
    }
}

module.exports = {
    Player,
    GameMap,
    MapCell,
};
