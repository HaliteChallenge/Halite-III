const hlt = require('./hlt');
const { Direction } = require('./hlt/positionals');
const logging = require('./hlt/logging');

const game = new hlt.Game();
game.initialize().then(async () => {
    await game.ready('MyJavaScriptBot');

    logging.info(`My Player ID is ${game.myId}.`);

    while (true) {
        await game.updateFrame();

        const { gameMap, me } = game;

        const commandQueue = [];

        for (const ship of me.getShips()) {
            if (ship.haliteAmount > 500) {

            }
            else if (gameMap.get(ship.position).haliteAmount < 100) {
                const direction = Direction.getAllCardinals()[Math.floor(4 * Math.random())];
                commandQueue.push(ship.move(direction));
            }
        }

        if (game.turnNumber === 0) {
            commandQueue.push(me.shipyard.spawn());
        }

        await game.endTurn(commandQueue);
    }
});
