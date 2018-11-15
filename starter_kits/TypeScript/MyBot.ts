import { Constants } from './hlt/constants';
import { Direction } from './hlt/positionals';
import { Logging } from './hlt/logging';
import { Game } from './hlt/networking';
import { GameMap, Player } from './hlt/gameMap';

const game = new Game();
game.initialize().then(async () => {
    // At this point "game" variable is populated with initial map data.
    // This is a good place to do computationally expensive start-up pre-processing.
    // As soon as you call "ready" function below, the 2 second per turn timer will start.
    await game.ready('MyJavaScriptBot');

    Logging.info(`My Player ID is ${game.myId}.`);

    while (true) {
        await game.updateFrame();

        const gameMap =  game.gameMap;
        const me = game.me;
        if(!(gameMap instanceof GameMap) || !(me instanceof Player)) {
            throw "Game not initialized properly!";
        }

        const commandQueue = [];

        for (const ship of me.getShips()) {
            if (ship.haliteAmount > Constants.MAX_ENERGY / 2) {
                const destination = me.shipyard.position;
                const safeMove = gameMap.naiveNavigate(ship, destination);
                commandQueue.push(ship.move(safeMove));
            }
            else if (gameMap.get(ship.position).haliteAmount < Constants.MAX_ENERGY / 10) {
                const direction = Direction.getAllCardinals()[Math.floor(4 * Math.random())];
                const destination = ship.position.directionalOffset(direction);
                const safeMove = gameMap.naiveNavigate(ship, destination);
                commandQueue.push(ship.move(safeMove));
            }
        }

        if (game.turnNumber < 0.75 * Constants.MAX_TURNS &&
            me.haliteAmount >= Constants.SHIP_COST &&
            !gameMap.get(me.shipyard).isOccupied) {
            commandQueue.push(me.shipyard.spawn());
        }

        await game.endTurn(commandQueue);
    }
});
