const hlt = require('./hlt');

const game = new hlt.Game();
game.initialize().then(async () => {
    await game.ready('MyJavaScriptBot');

    while (true) {
        await game.updateFrame();
        await game.endTurn([]);
    }
});
