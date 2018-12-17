// Extra helper functions to make developing a bot easier.
const utils = `
let currentBlock = null;
let numBlocks = 0;

function recordBlockBegin(id) {
    currentBlock = id;
}
function recordBlockEnd(id) {
    currentBlock = null;
}
function incrementCounter() {
    numBlocks++;
}

function mathRandomInt(a, b) {
    if (a > b) {
        [b, a] = [a, b];
    }
    return Math.floor(Math.random() * (b - a + 1) + a);
}

function atDropoff(gameMap, me, currentShip) {
    const cell = gameMap.get(currentShip.position);
    if (cell.hasStructure) {
        return cell.structure.owner === currentShip.owner;
    }
    return false;
}

function nearestShipyard(gameMap, me, currentShip) {
    let result = me.shipyard.position;
    let bestDistance = gameMap.calculateDistance(result, currentShip.position);

    for (const dropoff of me.getDropoffs()) {
        const distance = gameMap.calculateDistance(dropoff.position, currentShip.position);
        if (distance < bestDistance) {
            bestDistance = distance;
            result = dropoff.position;
        }
    }

    return result;
}

function nearestEnemy(game) {
    for (const player of game.players.values()) {
        if (player.id === game.myId) continue;
        if (player.getShips().length > 0) return true;
    }

    return false;
}

function nearestEnemy(game, currentShip) {
    let result = null;
    let bestDistance = 99999;

    for (const player of game.players.values()) {
        if (player.id === game.myId) continue;
        for (const ship of player.getShips()) {
            const distance = gameMap.calculateDistance(ship.position, currentShip.position);
            if (distance < bestDistance) {
                bestDistance = distance;
                result = ship;
            }
        }
    }

    return result;
}

/** Preprocess the map and find regions of high halite. */
function analyzeMap(gameMap, me) {
    gameMap.regions = [];

    const cellSize = 4;
    const horizCells = Math.floor(gameMap.width / cellSize);
    const vertCells = Math.floor(gameMap.height / cellSize);

    for (let cellX = 0; cellX < horizCells; cellX++) {
        for (let cellY = 0; cellY < vertCells; cellY++) {
            let totalHalite = 0;
            for (let dx = 0; dx < cellSize; dx++) {
                for (let dy = 0; dy < cellSize; dy++) {
                    const curPos = new Position(cellX * cellSize + dx,
                                                cellY * cellSize + dy);
                    totalHalite += gameMap.get(curPos).haliteAmount;
                }
            }
            gameMap.regions.push({
                position: new Position((cellX + 0.5) * cellSize,
                                       (cellY + 0.5) * cellSize),
                haliteAmount: totalHalite,
            });
        }
    }
}

/** Find the coordinates of a nearby region with halite. */
function nearestHalite(gameMap, me, currentShip) {
    let bestScore = 999999;
    let bestPosition = currentShip.position;

    for (const region of gameMap.regions) {
        let score = gameMap.calculateDistance(currentShip.position, region.position);
        score /= region.haliteAmount + 1;
        if (score < bestScore) {
            bestScore = score;
            bestPosition = region.position;
        }
    }
    return bestPosition;
}
`;

/**
 * Generate the source for a bot that runs in the browser.
 *
 * This contains the entire starter kit, inlined here.
 */
function generateBrowserBot(ships, shipyard) {
    const rawCode = `
(INPUT_BUFFER) => {
const hlt = {};
hlt.constants = {
    /**
     * The maximum number of map cells to consider in navigation.
     */
    MAX_BFS_STEPS: 1024,  // search an entire 32x32 region

    /**
     * Load constants from JSON given by the game engine.
     */
    loadConstants: function loadConstants(constants) {
        /** The cost to build a single ship. */
        this.SHIP_COST = constants['NEW_ENTITY_ENERGY_COST'];

        /** The cost to build a dropoff. */
        this.DROPOFF_COST = constants['DROPOFF_COST'];

        /** The maximum amount of halite a ship can carry. */
        this.MAX_HALITE = constants['MAX_ENERGY'];

        /**
         * The maximum number of turns a game can last. This reflects
         * the fact that smaller maps play for fewer turns.
        */
        this.MAX_TURNS = constants['MAX_TURNS'];

        /** 1/EXTRACT_RATIO halite (truncated) is collected from a square per turn. */
        this.EXTRACT_RATIO = constants['EXTRACT_RATIO'];

        /** 1/MOVE_COST_RATIO halite (truncated) is needed to move off a cell. */
        this.MOVE_COST_RATIO = constants['MOVE_COST_RATIO'];

        /** Whether inspiration is enabled. */
        this.INSPIRATION_ENABLED = constants['INSPIRATION_ENABLED'];

        /**
         * A ship is inspired if at least INSPIRATION_SHIP_COUNT
         * opponent ships are within this Manhattan distance.
        */
        this.INSPIRATION_RADIUS = constants['INSPIRATION_RADIUS'];

        /**
         * A ship is inspired if at least this many opponent ships are
         * within INSPIRATION_RADIUS distance.
        */
        this.INSPIRATION_SHIP_COUNT = constants['INSPIRATION_SHIP_COUNT'];

        /** An inspired ship mines 1/X halite from a cell per turn instead. */
        this.INSPIRED_EXTRACT_RATIO = constants['INSPIRED_EXTRACT_RATIO'];

        /** An inspired ship that removes Y halite from a cell collects X*Y additional halite. */
        this.INSPIRED_BONUS_MULTIPLIER = constants['INSPIRED_BONUS_MULTIPLIER'];

        /** An inspired ship instead spends 1/X% halite to move. */
        this.INSPIRED_MOVE_COST_RATIO = constants['INSPIRED_MOVE_COST_RATIO'];
    },
};
hlt.commands = (function() {
const NORTH = 'n';
const SOUTH = 's';
const EAST = 'e';
const WEST = 'w';
const STAY_STILL = 'o';
const GENERATE = 'g';
const CONSTRUCT = 'c';
const MOVE = 'm';

return {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    STAY_STILL,
    GENERATE,
    CONSTRUCT,
    MOVE,
};
})();

hlt.positionals = (function(commands) {
class Direction {
    constructor(dx, dy) {
        this.dx = dx;
        this.dy = dy;
    }

    equals(other) {
        return this.dx === other.dx && this.dy === other.dy;
    }

    toString() {
        return \`$\{this.constructor.name}($\{this.dx}, $\{this.dy})\`;
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
        throw new Error(\`Non-cardinal direction cannot be converted to wire format: $\{this}\`);
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
        throw new Error(\`Non-cardinal direction cannot be inverted: $\{this}\`);
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
        return \`$\{this.constructor.name}($\{this.x}, $\{this.y})\`;
    }
}

return {
    Direction,
    Position,
};
})(hlt.commands);

hlt.entity = (function(positionals, commands, constants) {
    const { Direction, Position } = positionals;


/** Base entity class for Ships, Dropoffs, and Shipyards. */
class Entity {
    constructor(owner, id, position) {
        this.owner = owner;
        this.id = id;
        this.position = position;
    }

    toString() {
        return \`$\{this.constructor.name}(id=$\{this.id}, $\{this.position})\`;
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
              .split(/\\s+/)
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
        return \`$\{commands.CONSTRUCT} $\{this.id}\`;
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
        return \`$\{commands.MOVE} $\{this.id} $\{direction}\`;
    }

    /**
     * Return a command to not move this ship.
     *
     * Not strictly needed, since ships do nothing by default.
     */
    stayStill() {
        return \`$\{commands.MOVE} $\{this.id} $\{commands.STAY_STILL}\`;
    }

    /**
     * Create a Ship instance for a player using the engine input.
     * @param playerId the owner
     * @return The ship ID and ship object.
     * @private
     */
    static async _generate(playerId, getLine) {
        const [ shipId, xPos, yPos, halite ] = (await getLine())
              .split(/\\s+/)
              .map(x => parseInt(x, 10));
        return [ shipId, new Ship(playerId, shipId, new Position(xPos, yPos), halite) ];
    }

    toString() {
        return \`$\{this.constructor.name}(id=$\{this.id}, $\{this.position}, cargo=$\{this.haliteAmount} halite)\`;
    }
}

return {
    Entity,
    Ship,
    Dropoff,
    Shipyard,
};
})(hlt.positionals, hlt.commands, hlt.constants);

hlt.gameMap = (function(constants, entity, positionals) {
const { Ship, Dropoff, Shipyard } = entity;
const { Direction, Position } = positionals;


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
              .split(/\\s+/)
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
        return \`MapCell($\{this.position}, halite=$\{this.haliteAmount})\`;
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
              .split(/\\s+/)
              .map(x => parseInt(x, 10));
        const gameMap = [];
        for (let i = 0; i < mapHeight; i++) {
            const row = [];
            row.fill(null, 0, mapWidth);
            gameMap.push(row);
        }

        for (let y = 0; y < mapHeight; y++) {
            const cells = (await getLine()).split(/\\s+/);
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
                  .split(/\\s+/)
                  .map(x => parseInt(x, 10));
            this.get(cellX, cellY).haliteAmount = cellEnergy;
        }
    }
}

return {
    Player,
    GameMap,
    MapCell,
};
})(hlt.constants, hlt.entity, hlt.positionals);

hlt.logging = ({
    log(...args) {},

    setup(filename) {
    },

    format(args) {
        return args.map(x => typeof x !== 'string' ? x.toString() : x).join(' ');
    },

    debug(...args) {
        this.log(\`DEBUG: $\{this.format(args)}\n\`);
    },

    info(...args) {
        this.log(\`INFO: $\{this.format(args)}\n\`);
    },

    warn(...args) {
        this.log(\`WARN: $\{this.format(args)}\n\`);
    },

    error(...args) {
        this.log(\`ERROR: $\{this.format(args)}\n\`);
    },
});

hlt.networking = (function(constants, logging, gameMap) {
const { GameMap, Player } = gameMap;


class Game {
    constructor() {
        this.turnNumber = 0;

        const getLine = function() {
            return new Promise(async (resolve) => {
                resolve(INPUT_BUFFER.shift());
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
              .split(/\\s+/)
              .map(tok => parseInt(tok, 10));
        this.myId = myId;

        this.players = new Map();
        for (let i = 0; i < numPlayers; i++) {
            this.players.set(i, await Player._generate(this._getLine));
        }
        this.me = this.players.get(this.myId);
        this.gameMap = await GameMap._generate(this._getLine);
    }

    /** Indicate that your bot is ready to play. */
    async ready(name) {
        await sendCommands(this.myId, [ name ]);
    }

    /**
     * Updates the game object's state.
     */
    async updateFrame() {
        this.turnNumber = parseInt(await this._getLine(), 10);
        logging.info(\`================ TURN $\{this.turnNumber.toString().padStart(3, '0')} ================\`);

        for (let i = 0; i < this.players.size; i++) {
            const [ player, numShips, numDropoffs, halite ] = (await this._getLine())
                  .split(/\\s+/)
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
        await sendCommands(this.myId, commands);
    }
}

/**
 * Sends a list of commands to the engine.
 * @param commands The list of commands to send.
 * @returns a Promise fulfilled once stdout is drained.
 */
function sendCommands(botId, commands) {
    return new Promise((resolve) => {
        SEND_COMMANDS(botId, commands);
        resolve();
    });
}

return {
    Player,
    Game,
    sendCommands,
};

})(hlt.constants, hlt.logging, hlt.gameMap);

hlt.Direction = hlt.positionals.Direction;
hlt.Position = hlt.positionals.Position;
hlt.Entity = hlt.entity.Entity;
hlt.Dropoff = hlt.entity.Dropoff;
hlt.Shipyard = hlt.entity.Shipyard;
hlt.Ship = hlt.entity.Ship;
hlt.Game = hlt.networking.Game;
hlt.Player = hlt.networking.Player;
hlt.MapCell = hlt.gameMap.MapCell;
hlt.GameMap = hlt.gameMap.GameMap;

const { Direction, Position } = hlt.positionals;
const logging = hlt.logging;

${utils}
const shipState = new Map();

const game = new hlt.Game();
return [
            async function(log) {
                logging.log = log;
                return game.initialize().then(async () => {
                    // At this point "game" variable is populated with initial map data.
                    // This is a good place to do computationally expensive start-up pre-processing.
                    // As soon as you call "ready" function below, the 2 second per turn timer will start.
                    return "MyJavaScriptBot";
                });
            },

            async function() {
                await game.updateFrame();

                const { gameMap, me } = game;

                analyzeMap(gameMap, me);

                const commandQueue = [];

                numBlocks = 0;

${ships}

${shipyard}

                return commandQueue;
            },
];

}
`;
    const code = prettier.format(rawCode, {
        parser: "babylon",
        plugins: prettierPlugins,
    });
    return code;
}

/**
 * Generate the standard bot for use with Halite.
 */
function generateStandardBot(ships, shipyard) {
    const rawCode = `
const hlt = require('./hlt');
const { Direction, Position } = require('./hlt/positionals');
const logging = require('./hlt/logging');

${utils}

const game = new hlt.Game();
game.initialize().then(async () => {
    // At this point "game" variable is populated with initial map data.
    // This is a good place to do computationally expensive start-up pre-processing.
    // As soon as you call "ready" function below, the 2 second per turn timer will start.
    await game.ready('MyJavaScriptBot');

    logging.info(\`My Player ID is $\{game.myId}.\`);

    const shipState = new Map();

    while (true) {
        await game.updateFrame();

        const { gameMap, me } = game;

        analyzeMap(gameMap, me);

        const commandQueue = [];

${ships}

${shipyard}

        await game.endTurn(commandQueue);
    }
});
`;
    const code = prettier.format(rawCode, {
        parser: "babylon",
        plugins: prettierPlugins,
    });
    return code;
}
