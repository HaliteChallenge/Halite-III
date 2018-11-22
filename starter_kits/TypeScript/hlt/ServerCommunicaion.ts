import { createInterface } from "readline";
import { Constants } from "./Constants";

interface InitialPlayerData { id: number, x: number, y: number };
interface InitialData { myId: number, players: InitialPlayerData[], cells: number[][] };
interface DropoffData { id: number, x: number, y: number };
interface ShipData { id: number, x: number, y: number, halite: number };
interface PlayerData { id: number, ships: ShipData[], dropoffs: DropoffData[], halite: number };
interface CellData { x: number, y: number, halite: number };
interface UpdateData { turn: number, players: PlayerData[], cells: CellData[] };

type Resolve =
    (value?: {} | PromiseLike<{}> | undefined) => void

/**
 * Transforms the asynchronous server communication to simple data objects
 * like InitialData and UpdateData.
 */
export class ServerCommunication {
    private buffer: string[] = [];
    private waitingForNewLine?: Promise<{}>;

    // We use here multiple Promises as async mechanism;
    // TODO: With node 10 we can use async generators mabe? 
    // https://medium.com/@segersian/howto-async-generators-in-nodejs-c7f0851f9c02

    constructor() {
        let newLineReceived: Resolve;

        const refreshAsyncSignalling = () => // Because promises fire once
            this.waitingForNewLine = new Promise(resolve => newLineReceived = resolve);
        refreshAsyncSignalling();

        const stdin = createInterface({ input: process.stdin, output: undefined });
        stdin.on('line', line => {
            line = line.trim();
            this.buffer.push(line);
            newLineReceived();
            refreshAsyncSignalling();
        });
    }

    async getLine() {
        while (this.buffer.length === 0) {
            await this.waitingForNewLine;
        }
        return <string>this.buffer.shift(); // Not undefined because we checked buffer.length
    }

    /**
     * Sends a list of commands to the engine.
     * @returns a Promise fulfilled once stdout is drained.
     */
    sendCommands(commands: string[]) {
        return new Promise((resolve) => {
            process.stdout.write(commands.join(' ') + '\n', () => resolve());
        });
    }

    private async getNumbers() {
        return (await this.getLine())
            .trim() // Server sends trailing spaces on cell data
            .split(/\s+/)
            .map(x => parseInt(x, 10));
    }

    async getDataRows(count: number, callback: (data: number[]) => void) {
        for (let i = 0; i < count; i++) {
            const row = await this.getNumbers();
            await callback(row);
        }
    }

    async getInitialData() {
        const rawConstants = await this.getLine();
        Constants.loadConstants(JSON.parse(rawConstants));

        const [numPlayers, myId] = await this.getNumbers();
        const data: InitialData = { myId, players: [], cells: [] };

        await this.getDataRows(numPlayers, ([id, x, y]) => {
            data.players.push({ id, x, y });
        });

        const [, mapHeight] = await this.getNumbers();
        await this.getDataRows(mapHeight, (row) => {
            data.cells.push(row);
        })

        return data;
    }

    async getUpdateData(numPlayers: number) {
        const data: UpdateData = { turn: 0, cells: [], players: [] };
        [data.turn] = await this.getNumbers();

        await this.getDataRows(numPlayers, async ([id, numShips, numDropoffs, halite]) => {
            const player: PlayerData = { id, ships: [], dropoffs: [], halite };

            await this.getDataRows(numShips, ([id, x, y, halite]) => {
                player.ships.push({ id, x, y, halite });
            })

            await this.getDataRows(numDropoffs, ([id, x, y]) => {
                player.dropoffs.push({ id, x, y });
            })
            data.players.push(player);
        });

        const [numChangedCells] = await this.getNumbers();
        await this.getDataRows(numChangedCells, ([x, y, halite]) => {
            data.cells.push({ x, y, halite });
        });

        return data;
    }
}