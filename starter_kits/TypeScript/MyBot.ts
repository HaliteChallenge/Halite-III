import { Constants } from "./hlt/Constants";
import { Direction } from "./hlt/Direction";
import { Game } from "./hlt/Game";
import { Logging } from "./hlt/Logging";
import { Random } from "./hlt/Random";

const firstArgument = process.argv[2];
const random = new Random(firstArgument ? Number(firstArgument) : 324231);

const game = new Game();
game.initialize().then(async ([gameMap, me]) => {
    // At this point "game" variable is populated with initial map data.
    // This is a good place to do computationally expensive start-up pre-processing.
    // As soon as you call "ready" function below, the 2 second per turn timer will start.
    await game.ready("MyTypeScriptBot");

    Logging.info(`My Player ID is ${game.myId}.`);

    while (true) {
        await game.updateFrame();

        const commandQueue = [];

        // Movement algorithm parameters
        const fullCargoCoefficient = 0.5;
        const tooLittleHaliteToKeepHarvestingCoefficient = 0.1;
        const spawnNewShipsUntilGameTurnCoefficient = 0.75;

        for (const ship of me.getShips()) {
            if (ship.haliteAmount > fullCargoCoefficient * Constants.MAX_ENERGY) {
                // Go home if full
                const destination = me.shipyard.position;
                const safeMove = gameMap.naiveNavigate(ship, destination);
                commandQueue.push(ship.move(safeMove));
            } else if (
                gameMap.get(ship.position).haliteAmount <
                tooLittleHaliteToKeepHarvestingCoefficient * Constants.MAX_ENERGY
            ) {
                // Go harvest to a new place
                const getRandomArrayElement = (anArray: any[]) => anArray[Math.floor(anArray.length * random.next())];
                const direction = getRandomArrayElement(Direction.getAllCardinals());
                const destination = ship.position.directionalOffset(direction);
                const safeMove = gameMap.naiveNavigate(ship, destination);
                commandQueue.push(ship.move(safeMove));
            } else {
                // Keep still and harvest in place
            }
        }

        if (game.turnNumber < spawnNewShipsUntilGameTurnCoefficient * Constants.MAX_TURNS &&
            // Spawn new ships
            me.haliteAmount >= Constants.SHIP_COST &&
            !gameMap.get(me.shipyard).isOccupied) {
            commandQueue.push(me.shipyard.spawn());
        }

        await game.endTurn(commandQueue);
    }
});
