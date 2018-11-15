import { createInterface } from "readline";
import { Constants } from "./constants";
import { Logging } from "./logging";
import { Player, GameMap } from "./gameMap";


export class Game {
    turnNumber: number;

    _getLine: () => Promise<string>;
    public myId: number = 0;
    public players = new Map<number, Player>();
    public me: Player | undefined = undefined;
    public gameMap: GameMap | undefined = undefined;

    // We use here multiple Promises as async mechanism;
    // With node 10 we can use async generators mabe? 
    // TODO: https://medium.com/@segersian/howto-async-generators-in-nodejs-c7f0851f9c02
    constructor() {
        this.turnNumber = 0;

        // Setup input/output
        const rl = createInterface({
            input: process.stdin,
            output: undefined,
        });
        const buffer: string[] = [];
        let currentResolve: (value?: {} | PromiseLike<{}> | undefined) => void;
        const makePromise = function () {
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
        const getLine = () => {
            return new Promise(async (resolve:(value: string) => void) => {
                while (buffer.length === 0) {
                    await currentPromise;
                }
                resolve(<string>buffer.shift()); // With the above condition we guarantie that we return a sting
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
        Constants.loadConstants(JSON.parse(rawConstants));

        const [numPlayers, myId] = (await this._getLine())
            .split(/\s+/)
            .map(token => parseInt(token, 10));
        this.myId = myId;

        Logging.setup(`bot-${myId}.log`);

        for (let i = 0; i < numPlayers; i++) {
            this.players.set(i, await Player._generate(this._getLine));
        }
        this.me = this.players.get(this.myId);
        this.gameMap = await GameMap._generate(this._getLine);
    }

    /** Indicate that your bot is ready to play. */
    async ready(name: string) {
        await sendCommands([name]);
    }

    /**
     * Updates the game object's state.
     */
    async updateFrame() {
        this.turnNumber = parseInt(await this._getLine(), 10);
        Logging.info(`================ TURN ${this.turnNumber.toString().padStart(3, '0')} ================`);
        const f = new String("dsf");

        for (let i = 0; i < this.players.size; i++) {
            const [playerId, numShips, numDropoffs, halite] = (await this._getLine())
                .split(/\s+/)
                .map(x => parseInt(x, 10));
            const player = this.players.get(playerId);
            if(player instanceof Player) {
                await (<Player>this.players.get(playerId))._update(numShips, numDropoffs, halite, this._getLine);
            } else {
                throw "Game.updateFrame got wrong player Id from the stdin or Game is not initialized yet!";
            }
        }

        if(this.gameMap instanceof GameMap) {
            await (this.gameMap)._update(this._getLine);
        } else {
            throw "Game.updateFrame() called when Game is not initialized!";
        }

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
     * @param commands
     */
    async endTurn(commands: string[]) {
        await sendCommands(commands);
    }
}

/**
 * Sends a list of commands to the engine.
 * @param commands The list of commands to send.
 * @returns a Promise fulfilled once stdout is drained.
 */
function sendCommands(commands: string[]) {
    return new Promise((resolve) => {
        process.stdout.write(commands.join(' ') + '\n', function () {
            resolve();
        });
    });
}
