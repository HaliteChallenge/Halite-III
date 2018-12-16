let numPlayers = 2;
let botCode = '';
let players = [];
let inputBuffers = [];
let turnFuncs = [];
let RUN_BOT = null;

self.onmessage = async function(msg) {
    console.log('worker', msg);

    if (msg.data.action === 'init') {
        ({ numPlayers, botCode } = msg.data);
        RUN_BOT = eval(botCode);

        hlt.Logging.set_level(hlt.LoggingLevel.Info);
        const map = new hlt.Map(32, 32);
        const mapgen = {
            type: hlt.MapgenMapType.Fractal,
            seed: Math.floor(Date.now() / 1000),
            width: 32,
            height: 32,
            num_players: numPlayers,
        };
        hlt.Mapgen.generate(map, mapgen);
        const nc = new hlt.NetworkingConfig();
        const gs = new hlt.GameStatistics();
        const replay = new hlt.Replay(gs, numPlayers, 0, map);
        const halite = new hlt.Halite(map, nc, gs, replay);
        const impl = halite.get_impl();
        const snapshot = new hlt.Snapshot();
        const cmds = new hlt.VectorString();
        cmds.push_back("player 1");
        cmds.push_back("player 2");
        impl.initialize_game(cmds, snapshot);
        const net = halite.get_networking();
        for (let i = 0; i < numPlayers; i++) {
            const playerId = halite.get_store().get_player(new hlt.PlayerId(i));
            players.push(playerId);
            const buf = net.generate_player_init_input(playerId).split(/\n/g).filter(x=>x);
            inputBuffers.push(buf);
            const [ init, turn ] = RUN_BOT(buf);
            const name = await init((logMessage) => {
                self.postMessage({
                    status: 'message',
                    message: `[LOG] [P${i}] ${logMessage.toString().trim()}`,
                });
            });
            console.log('Initialized', name);
            turnFuncs.push(turn);
        }

        impl.initialize_players();

        for (let i = 1; i <= impl.max_turns(); i++) {
            impl.set_turn_number(i);
            const input = halite.get_networking().generate_player_input();
            input.split(/\n/g).filter(x=>x).forEach(line => {
                for (const buf of inputBuffers) buf.push(line);
            });

            const turn_commands = new hlt.MapPlayerIdString();
            let j = 0;
            for (const turnFunc of turnFuncs) {
                let cmds = [];
                try {
                    cmds = await turnFunc();
                }
                catch (e) {
                    // Bot errored
                    console.warn('Bot errored', e);
                    self.postMessage({
                        status: 'message',
                        message: `[Player ${j}] ERROR: ${e}`,
                    });
                    console.log(e);
                    impl.kill_player(new hlt.PlayerId(j));
                }
                console.log('Player', j, 'commands:', cmds);
                turn_commands.set(new hlt.PlayerId(j), cmds.join(' ') + '\n');
                j += 1;
            }
            impl.start_turn();
            impl.process_turn_from_input(turn_commands);
            impl.end_turn();

            if (impl.game_ended()) {
                break;
            }
        }
        impl.end_game();
        self.postMessage({
            status: 'done',
            replay: replay.as_json_string(),
        });
    }
};

self.importScripts('halite_web/halite_web.js');

self.HaliteWeb({
    locateFile(path) {
        return 'halite_web/' + path;
    },
}).then((hlt) => {
    self.hlt = hlt;

    function flush(utf8ArrayToString, tty) {
        const text = utf8ArrayToString(tty.output, 0);
        self.postMessage({
            status: 'message',
            message: text,
        });
        tty.output = [];
    }

    function wrapEmscripten(utf8ArrayToString, fs) {
        fs.tty.ops.flush = (tty) => {
            if (tty.output && tty.output.length > 0) {
                flush(utf8ArrayToString, tty);
            }
        };
        fs.tty.ops.put_char = (tty, val) => {
            if (val === null || val === 10) {
                flush(utf8ArrayToString, tty);
            }
            else if (val !== 0) {
                tty.output.push(val);
            }
        };
    }

    self.postMessage({ status: 'ready' });

    wrapEmscripten(hlt.UTF8ArrayToString, hlt.FS.streams[1]);
    wrapEmscripten(hlt.UTF8ArrayToString, hlt.FS.streams[2]);
});
