import * as libhaliteviz from '../../../libhaliteviz';

function generateBotMain(workspace, traceBlockly=false) {
    let shipyard = '';
    let ships = '';

    if (traceBlockly) {
        // Track # of blocks that execute & what block is executing
        Blockly.JavaScript.STATEMENT_PREFIX = "recordBlockBegin(%1);\nincrementCounter();\n";
        Blockly.JavaScript.STATEMENT_POSTFIX = "recordBlockEnd(%1);\n";
    }

    for (const block of workspace.getTopBlocks()) {
        if (block.type === 'shipyard') {
            shipyard = Blockly.JavaScript.blockToCode(block);
        }
        else if (block.type === 'each_ship') {
            ships = Blockly.JavaScript.blockToCode(block);
        }
    }

    return { ships, shipyard };
}

function debounce(f, interval) {
    let timer = null;
    return function(...args) {
        if (timer) {
            window.clearTimeout(timer);
        }
        timer = window.setTimeout(function() {
            f(...args);
        }, interval);
    };
}

function validateBlocks(workspace) {
    for (const block of workspace.getAllBlocks()) {
        if (block.getParent()) {
            if (block.actionType && block.actionType !== block.getRootBlock().type) {
                console.log(block.actionType, block.getRootBlock().type);
                block.setWarningText('This block is inside the wrong block kind.');
            }
            else {
                block.setWarningText(null);
            }
        }
        else if (block.type !== 'shipyard' && block.type !== 'each_ship') {
            block.setWarningText('This block must be inside a "my shipyard" or "my ships" block.');
        }
        // block.getRootBlock
    }
}

function generateBrowserCode(workspace) {
    const { ships, shipyard } = generateBotMain(workspace, true);
    console.log(ships);
    console.log(shipyard);
    return generateBrowserBot(ships, shipyard);
}

function showGame(replay) {
    const container = document.querySelector('#visualizer');
    container.innerHTML = '';
    const viz = new libhaliteviz.EmbeddedVisualizer(replay, 450, 450);
    viz.attach(container);
    viz.play();
}

const BOT_README = `This is your Blockly bot!

Want to see how your bot fares in the Halite challenge? Just register
at https://halite.io, then drag-and-drop this zip file into the Play page.
`;

async function generateZip(workspace) {
    const { ships, shipyard } = generateBotMain(workspace);
    const main = generateStandardBot(ships, shipyard);
    const req = await window.fetch('/assets/downloads/Halite3_JavaScript_None.zip');
    const blob = await req.blob();
    const zipFile = await JSZip.loadAsync(blob);
    zipFile.file("MyBot.js", main);
    zipFile.file("LANGUAGE", "Blockly/JavaScript");
    zipFile.file("README_BLOCKLY.txt", BOT_README);
    return zipFile;
}

function exportBot(workspace) {
    generateZip(workspace)
        .then((zipFile) => zipFile.generateAsync({ type: 'blob' }))
        .then((result) => saveAs(result, 'MyJavaScriptBot.zip'));
}

function apiUrl(url) {
    return `https://api.2018.halite.io/v1/${url}`;
}

async function fetchApi(endpoint, params) {
    const response = await window.fetch(apiUrl(endpoint), Object.assign({}, params, {
        credentials: 'include',
    }));
    return await response.json();
}

function submitBot(workspace) {
    (async function() {
        saveBot(workspace);
        const user = await fetchApi('login/me');
        if (!user) {
            document.querySelector('#login-dialog').classList.add('shown');
            return;
        }

        const bots = await fetchApi(`api/user/${user.user_id}/bot`);
        const formData = new window.FormData();
        const zipFile = await generateZip(workspace);
        const blob = await zipFile.generateAsync({ type: 'blob' });
        formData.append('name', 'botFile');
        formData.append('botFile', blob);
        let response;
        if (bots.length > 0) {
            response = await fetchApi(`api/user/${user.user_id}/bot/${bots[0].bot_id}`, {
                method: 'PUT',
                body: formData,
            });
        }
        else {
            response = await fetchApi(`api/user/${user.user_id}/bot`, {
                method: 'POST',
                body: formData,
            });
        }

        console.log(response);
        const banner = document.querySelector('#banner');
        if (response.status === 'success') {
            banner.innerText = 'Bot uploaded!';
        }
        else {
            banner.innerText = 'There was an error submitting your bot. Please try again later.';
        }
        banner.classList.add('shown');
        window.setTimeout(function() {
            banner.classList.remove('shown');
        }, 2500);
    })();
}

function saveBot(workspace) {
    const xml = Blockly.Xml.workspaceToDom(workspace);
    const xmlText = Blockly.Xml.domToText(xml);
    window.localStorage.setItem('workspace', xmlText);
    document.querySelector('#saved').style.opacity = '1';
}

window.fetch('/learn-programming-challenge/blockly/toolbox.xml').then(r => r.text()).then(toolboxText => {
    const hltConsole = new Console(document.querySelector('#console'));
    hltConsole.writeLine('Build a Halite bot with blocks!');
    hltConsole.writeLine('Run Halite games with your bot, and iterate.');
    hltConsole.writeLine('When you’re ready, submit to enter it into');
    hltConsole.writeLine('the global competition.');
    hltConsole.writeLine('You can learn more about block functionality');
    hltConsole.writeLine('by hovering over each block.');
    hltConsole.writeLine('To learn more about the Halite game,');
    hltConsole.writeLine('click “Game Rules”.');
    hltConsole.writeLine('=======================================');
    hltConsole.writeLine('Happy hacking!');
    const workspace = Blockly.inject(
        'editor',
        {
            toolbox: toolboxText,
        }
    );

    // Starter bot
    let xml = '<xml xmlns="http://www.w3.org/1999/xhtml"><block type="shipyard" id="g.Tdo{@)hV$`hDG;a=?{" deletable="false" x="23" y="20"><statement name="behavior"><block type="controls_if" id="!v$n!yPjYh~+{|5/._#X"><value name="IF0"><block type="logic_operation" id="dkuWAc|pDj{1lsS5?lA}"><field name="OP">AND</field><value name="A"><block type="nothing_on_top" id="/^rmV}I~9VwyG7!M]dPA"></block></value><value name="B"><block type="logic_compare" id="?Y:2!xF#NIEPHa@GsKFL"><field name="OP">GTE</field><value name="A"><block type="my_halite" id="K::VhO0eESU?pi=8qAKf"></block></value><value name="B"><block type="ship_cost" id="W(W;^NlLFABpgAS8~h~@"></block></value></block></value></block></value><statement name="DO0"><block type="spawn_ship" id="vOtm{csA4A3D4l0Mj5y8"></block></statement></block></statement></block><block type="each_ship" id="vlXL|`Dm}zaNmRn*Dln/" deletable="false" x="22" y="184"><statement name="commands"><block type="move_unsafe" id="Y@kwO)KRe!/B,KCli2_:"><value name="NAME"><block type="random_direction" id="ED)z4t`hWp:kLks8j!#="></block></value></block></statement></block></xml>';
    if (window.localStorage.getItem('workspace')) {
        xml = window.localStorage.getItem('workspace');
    }
    Blockly.Xml.domToWorkspace(Blockly.Xml.textToDom(xml), workspace);

    workspace.addChangeListener(function() {
        document.querySelector('#saved').style.opacity = '0';
    });
    workspace.addChangeListener(debounce(function onChanged() {
        validateBlocks(workspace);
        saveBot(workspace);
    }, 500));

    libhaliteviz.setAssetRoot('').then(() => {
        document.querySelector('#run-game').addEventListener('click', () => runGame());
        document.querySelector('#export-bot').addEventListener('click', () => exportBot(workspace));
        document.querySelector('#submit-bot').addEventListener('click', () => submitBot(workspace));
    });

    function runGame() {
        const container = document.querySelector('#visualizer');
        container.innerHTML = '';

        hltConsole.clear();
        const botCode = generateBrowserCode(workspace);
        const numPlayers = 2;
        const worker = new window.Worker('/learn-programming-challenge/blockly/worker.js');
        worker.onmessage = function(msg) {
            if (msg.data.status === 'ready') {
                console.log('Worker ready');
                worker.postMessage({
                    action: 'init',
                    numPlayers,
                    botCode,
                });
            }
            else if (msg.data.status === 'message') {
                hltConsole.writeLine(msg.data.message);
            }
            else if (msg.data.status === 'done') {
                const replay = JSON.parse(msg.data.replay);
                showGame(replay);
                console.log(replay);
                hltConsole.writeLine('');
                hltConsole.writeLine('Game complete!');
                for (const player of replay.game_statistics.player_statistics) {
                    hltConsole.writeLine(`Player ${player.player_id} was rank ${player.rank} with ${player.final_production} halite.`);
                }
            }
        };
    }
});
