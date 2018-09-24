const readline = require('readline');

const constants = require('./constants');
const logging = require('./logging');
const { GameMap, Player } = require('./gameMap');

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
        await sendCommands([ name ]);
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
        await sendCommands(commands);
    }
}

/**
 * Sends a list of commands to the engine.
 * @param commands The list of commands to send.
 * @returns a Promise fulfilled once stdout is drained.
 */
function sendCommands(commands) {
    return new Promise((resolve) => {
        process.stdout.write(commands.join(' ') + '\n', function() {
            resolve();
        });
    });
}

module.exports = {
    Player,
    Game,
    sendCommands,
};
