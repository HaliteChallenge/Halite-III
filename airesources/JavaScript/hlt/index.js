const readline = require('readline');

const commands = require('./commands');
const constants = require('./constants');
const logging = require('./logging');
const networking = require('./networking');

const { Direction, Position } = require('./positionals');

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

class Dropoff extends Entity {
    static async _generate(playerId, getLine) {
        const [ id, xPos, yPos ] = (await getLine())
              .split(/\s+/)
              .map(x => parseInt(x, 10));
        return [ id, new Dropoff(playerId, id, new Position(xPos, yPos)) ];
    }
}

class Shipyard extends Entity {
    spawn() {
        return commands.GENERATE;
    }
}

class Ship extends Entity {
    constructor(owner, id, position, haliteAmount) {
        super(owner, id, position);
        this.haliteAmount = haliteAmount;
    }

    get isFull() {
        return this.haliteAmount >= constants.MAX_HALITE;
    }

    makeDropoff() {
        return `${commands.CONSTRUCT} ${this.id}`;
    }

    stayStill() {
        return `${commands.MOVE} ${this.id} ${commands.STAY_STILL}`;
    }

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

    async initialize() {
        const rawConstants = await this._getLine();
        constants.loadConstants(JSON.parse(rawConstants));

        const [ numPlayers, myId ] = (await this._getLine())
              .split(/\s+/)
              .map(tok => parseInt(tok, 10));
        this.myId = myId;

        // TODO: LOGGING
        logging.setup(`bot-${myId}.log`);

        this.players = new Map();
        for (let i = 0; i < numPlayers; i++) {
            this.players.set(i, await Player._generate(this._getLine));
        }
        this.me = this.players.get(this.myId);
        this.gameMap = await GameMap._generate(this._getLine);
    }

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
    }

    async endTurn(commands) {
        await networking.sendCommands(commands);
    }
}

class Player {
    constructor(playerId, shipyard, halite=0) {
        this.id = playerId;
        this.shipyard = shipyard;
        this.haliteAmount = halite;
        this._ships = new Map();
        this._dropoffs = new Map();
    }

    getShip(shipId) {

    }

    getShips() {

    }

    getDropoff(dropoffId) {

    }

    getDropoffs() {

    }

    static async _generate(getLine) {
        const line = await getLine();
        const [ playerId, shipyardX, shipyardY ] = line
              .split(/\s+/)
              .map(x => parseInt(x, 10));
        return new Player(playerId,
                          new Shipyard(playerId, -1, new Position(shipyardX, shipyardY)));
    }

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

class MapCell {
    constructor(position, halite) {
        this.position = position;
        this.haliteAmount = halite;
        this.ship = null;
        this.structure = null;
    }

    get isEmpty() {

    }

    get isOccupied() {

    }

    get hasStructure() {

    }

    get structureType() {

    }

    markUnsafe(ship) {

    }

    equals(other) {

    }

    toString() {

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
        const [ location ] = args;
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
        while (x < 0) position.x += this.width;
        while (y < 0) position.y += this.height;
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

    _bfsTraverseSafely(source, direction) {

    }

    _findFirstMove(source, destination, visited) {

    }

    getSafeMove(source, destination) {

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
            logging.info(`'${line}' ${cellX} ${cellY} ${cellEnergy}`);
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
};
