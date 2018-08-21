const readline = require('readline');

const commands = require('./commands');
const constants = require('./constants');
const logging = require('./logging');
const networking = require('./networking');

const { Direction, Position } = require('./positionals');

/** Base entity class for Ships, Dropoffs, and Shipyards. */
class Entity {
    constructor(owner, id, position) {
        this.owner = owner;
        this.id = id;
        this.position = position;
    }

    toString() {
        return `${this.constructor.name}(id=${this.id}, ${this.position})`;
    }
}

/** Represents a dropoff. */
class Dropoff extends Entity {
    /**
     * Create a Dropoff for a specific player from the engine input.
     * @private
     * @param playerId the player that owns this dropoff
     * @param {Function} getLine function to read a line of input
     * @returns {Dropoff}
     */
    static async _generate(playerId, getLine) {
        const [ id, xPos, yPos ] = (await getLine())
              .split(/\s+/)
              .map(x => parseInt(x, 10));
        return [ id, new Dropoff(playerId, id, new Position(xPos, yPos)) ];
    }
}

/** Represents a shipyard. */
class Shipyard extends Entity {
    /** Return a move to spawn a new ship at your shipyard. */
    spawn() {
        return commands.GENERATE;
    }
}

/** Represents a ship. */
class Ship extends Entity {
    constructor(owner, id, position, haliteAmount) {
        super(owner, id, position);
        this.haliteAmount = haliteAmount;
    }

    /** Is this ship at max halite capacity? */
    get isFull() {
        return this.haliteAmount >= constants.MAX_HALITE;
    }

    /** Return a move to turn this ship into a dropoff. */
    makeDropoff() {
        return `${commands.CONSTRUCT} ${this.id}`;
    }

    /**
     * Return a command to move this ship in a direction without
     * checking for collisions.
     * @param {String|Direction} direction the direction to move in
     * @returns {String} the command
     */
    move(direction) {
        if (direction.toWireFormat) {
            direction = direction.toWireFormat();
        }
        return `${commands.MOVE} ${this.id} ${direction}`;
    }

    /**
     * Return a command to not move this ship.
     *
     * Not strictly needed, since ships do nothing by default.
     */
    stayStill() {
        return `${commands.MOVE} ${this.id} ${commands.STAY_STILL}`;
    }

    /**
     * Create a Ship instance for a player using the engine input.
     * @param playerId the owner
     * @return The ship ID and ship object.
     * @private
     */
    static async _generate(playerId, getLine) {
        const [ shipId, xPos, yPos, halite ] = (await getLine())
              .split(/\s+/)
              .map(x => parseInt(x, 10));
        return [ shipId, new Ship(playerId, shipId, new Position(xPos, yPos), halite) ];
    }

    toString() {
        return `${this.constructor.name}(id=${this.id}, ${this.position}, cargo=${this.haliteAmount} halite)`;
    }
}

class Game {
    constructor() {
        this.turnNumber = 0;

        // Setup input/output
        const rl = readline.createInterface({
            input: process.stdin,
            output: null,
        });
        const buffer = [];
        let currentResolve;
        const makePromise = function() {
            return new Promise((resolve) => {
                currentResolve = resolve;
            });
        };
        let currentPromise = makePromise();
        rl.on('line', (line) => {
            buffer.push(line);
            currentResolve();
            currentPromise = makePromise();
        });
        const getLine = function() {
            return new Promise(async (resolve) => {
                while (buffer.length === 0) {
                    await currentPromise;
                }
                resolve(buffer.shift());
            });
        };
        this._getLine = getLine;
    }

    /**
     * Initialize a game object collecting all the start-state
     * instances for pre-game. Also sets up a log file in
     * "bot-<bot_id>.log".
     */
    async initialize() {
        const rawConstants = await this._getLine();
        constants.loadConstants(JSON.parse(rawConstants));

        const [ numPlayers, myId ] = (await this._getLine())
              .split(/\s+/)
              .map(tok => parseInt(tok, 10));
        this.myId = myId;

        logging.setup(`bot-${myId}.log`);

        this.players = new Map();
        for (let i = 0; i < numPlayers; i++) {
            this.players.set(i, await Player._generate(this._getLine));
        }
        this.me = this.players.get(this.myId);
        this.gameMap = await GameMap._generate(this._getLine);
    }

    /** Indicate that your bot is ready to play. */
    async ready(name) {
        await networking.sendCommands([ name ]);
    }

    /**
     * Updates the game object's state.
     */
    async updateFrame() {
        this.turnNumber = parseInt(await this._getLine(), 10);
        logging.info(`================ TURN ${this.turnNumber.toString().padStart(3, '0')} ================`);

        for (let i = 0; i < this.players.size; i++) {
            const [ player, numShips, numDropoffs, halite ] = (await this._getLine())
                  .split(/\s+/)
                  .map(x => parseInt(x, 10));
            await this.players.get(player)._update(numShips, numDropoffs, halite, this._getLine);
        }

        await this.gameMap._update(this._getLine);

        // Mark cells with ships as unsafe for navigation

        for (const player of this.players.values()) {
            for (const ship of player.getShips()) {
                this.gameMap.get(ship.position).markUnsafe(ship);
            }
            this.gameMap.get(player.shipyard.position).structure = player.shipyard;
            for (const dropoff of player.getDropoffs()) {
                this.gameMap.get(dropoff.position).structure = dropoff;
            }
        }
    }

    /**
     * Send all commands to the game engine, effectively ending your
     * turn.
     * @param {Array} commands
     */
    async endTurn(commands) {
        await networking.sendCommands(commands);
    }
}

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

    /** Get an iterator over all of the player's ships. */
    getShips() {
        return this._ships.values();
    }

    /** Get a single dropoff by its ID. */
    getDropoff(dropoffId) {
        return this._dropoffs.get(dropoffId);
    }

    /** Get an iterator over all of the player's dropoffs. */
    getDropoffs() {
        return this._dropoffs.values();
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
        let x = position.x % this.width;
        let y = position.y % this.height;
        while (x < 0) x += this.width;
        while (y < 0) y += this.height;
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
     * Use a BFS to traverse the map safely, storing each previous
     * cell in a visited cell.
     * @param {MapCell} source
     * @param {MapCell} destination
     * @return {Array|null}
     */
    _bfsTraverseSafely(source, destination) {
        const visitedMap = [];
        for (let i = 0; i < this.height; i++) {
            const row = [];
            for (let j = 0; j < this.width; j++) {
                row.push(null);
            }
            visitedMap.push(row);
        }

        const potentialsQueue = [source];
        let stepsTaken = 0;

        while (potentialsQueue.length > 0) {
            const currentSquare = potentialsQueue.shift();
            if (currentSquare.equals(destination)) {
                return visitedMap;
            }

            for (let suitor of currentSquare.position.getSurroundingCardinals()) {
                suitor = this.normalize(suitor);
                if (!this.get(suitor).isOccupied && !visitedMap[suitor.y][suitor.x]) {
                    potentialsQueue.push(this.get(suitor));
                    visitedMap[suitor.y][suitor.x] = currentSquare;
                }
            }

            // logging.info(`${stepsTaken}`);
            stepsTaken++;
            if (stepsTaken >= constants.MAX_BFS_STEPS) {
                break;
            }
        }

        return null;
    }

    /**
     * Given a visited map, find the viable first move near the source
     * and return it
     * @param {MapCell} source
     * @param {MapCell} destination
     * @param {Array} visisted
     */
    _findFirstMove(source, destination, visited) {
        let currentSquare = destination;
        let previous = null;

        while (currentSquare !== null && !currentSquare.equals(source)) {
            logging.info(`csq ${currentSquare}`);
            previous = currentSquare;
            currentSquare = visited[currentSquare.position.y][currentSquare.position.x];
        }

        return previous;
    }

    /**
     * Returns a singular safe move towards the destination.
     * @param {Position} source
     * @param {Position} destination
     * @return {Direction|null}
     */
    _naiveNavigate(source, destination) {
        for (const direction of this.getUnsafeMoves(source, destination)) {
            const targetPos = source.directionalOffset(direction);

            if (!this.get(targetPos).isOccupied) {
                return direction;
            }
        }

        return null;
    }

    /**
     * Returns the best (read: most optimal) singular safe move
     * towards the destination.
     * @param {MapCell} source
     * @param {MapCell} destination
     * @returns {Direction|null}
     */
    getSafeMove(source, destination) {
        if (!(source instanceof MapCell && destination instanceof MapCell)) {
            throw new Error('source and destination must be of type MapCell');
        }

        if (source.equals(destination)) {
            return null;
        }

        const visitedMap = this._bfsTraverseSafely(source, destination);
        if (!visitedMap) {
            return this._naiveNavigate(source.position, destination.position);
        }

        const safeTargetCell = this._findFirstMove(source, destination, visitedMap);
        if (!safeTargetCell) {
            return null;
        }

        const potentialMoves = this.getUnsafeMoves(source.position, safeTargetCell.position);
        if (!potentialMoves) {
            return null;
        }

        return potentialMoves[0];
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
    Entity,
    Dropoff,
    Shipyard,
    Ship,
    Game,
    Player,
    MapCell,
    GameMap,
    constants,
    logging,
};
