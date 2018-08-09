<template>
    <section id="local-bot">
        <header>
            <h2>Select Local Bot</h2>
        </header>
        <template v-if="localBot">
            <p>Bot path: {{localBot}}</p>
            <button @click="benchmark">Benchmark</button>
            <button>Gym</button>
            <button @click="upload">Upload</button>
        </template>
        <template v-else>
            <p>
                Choose a bot to benchmark it against our bots,
                evaluate its performance vs your currently uploaded
                bot, or upload it to the server.
            </p>
            <button v-on:click="chooseLocalBot">Select Bot</button>
        </template>
    </section>
</template>

<script>
    import { remote as electronRemote } from 'electron';
    import fs from 'fs';
    import path from 'path';
    import JSZip from 'jszip';

    import * as util from './util';

    export default {
        props: ['apiKey', 'userId'],
        inject: ['showModal', 'closeModal'],
        data() {
            return {
                localBot: null,
            };
        },
        methods: {
            chooseLocalBot() {
                const [ path ] = electronRemote.dialog.showOpenDialog({
                    title: 'Choose bot folder or MyBot file',
                    properties: ['openFile', 'openDirectory'],
                });
                this.localBot = path;
            },

            async prepareAssets() {
                const dataDir = path.join(electronRemote.app.getPath('appData'),
                                          'halite-in-a-box');
                try {
                    fs.mkdirSync(dataDir);
                }
                catch (e) {
                    if (e.code && e.code === 'EEXIST') {
                        // Already exists, do nothing
                    }
                    else {
                        console.error(e);
                        throw e;
                    }
                }

                console.info(`Data directory: ${dataDir}`);

                let platform;
                let environmentBinary;
                if (process.platform === 'darwin') {
                    platform = 'MacOS';
                    environmentBinary = 'halite';
                }
                else if (process.platform === 'win32') {
                    platform = 'Windows-AMD64';
                    environmentBinary = 'halite.exe';
                }
                else if (process.platform === 'linux') {
                    platform = 'Linux-AMD64';
                    environmentBinary = 'halite';
                }

                const environmentPath = path.join(dataDir, environmentBinary);
                if (!fs.existsSync(environmentPath)) {
                    console.info(`Downloading environment to ${environmentPath}`);

                    const request = await util.download(`assets/downloads/Halite2_${platform}.zip`);
                    const rawZip = await request.arrayBuffer();
                    const zip = await JSZip.loadAsync(rawZip);
                    const binary = await zip.file(environmentBinary).async('uint8array');
                    await util.writeFile(environmentPath, binary);
                    fs.chmodSync(environmentPath, 0o775);
                }

                return {
                    environmentPath,
                };
            },

            async benchmark() {
                const action = await this.showModal('benchmark-modal', {
                    status: 'setup',
                });
                console.log(action);
                if (action === 'cancel') {
                    this.closeModal();
                    return;
                }

                // TODO: autodetect/embed interpreter, environment
                const params = ['gym', '-i', action.games,
                                '-b', '/Users/lidavidm/Code/Halite-III-Internal/environment/halite',
                                '-r', 'python3 ' + this.localBot];
                // TODO: input validation
                for (const bot of action.bots) {
                    params.push('-r');
                    if (bot === 'self') {
                        params.push('python3 ' + this.localBot);
                    }
                }

                const state = {
                    status: 'running',
                    gamesPlayed: 0,
                    gamesTotal: action.games,
                    stats: [],
                };

                this.showModal('benchmark-modal', state);

                for await (const value of util.call(params)) {
                    console.log(value);
                    if (value.games_played) {
                        state.gamesPlayed = value.games_played;
                    }
                    if (value.stats) {
                        for (const [stringKey, gamesWon] of Object.entries(value.stats)) {
                            state.stats[parseInt(stringKey, 10)] = gamesWon;
                        }
                    }
                    this.showModal('benchmark-modal', state);
                }

                state.status = 'finished';
                await this.showModal('benchmark-modal', state);
                this.closeModal();
            },

            async upload() {
                let items = null;
                const params = ['bot', '-b', this.localBot, 'upload', '--dry-run'];
                for await (const value of util.call(params)) {
                    if (value.items) {
                        items = value.items;
                    }
                }
                const operation = await this.showModal('upload-modal', {
                    status: 'previewing',
                    localBot: this.localBot,
                    files: items,
                });

                if (operation === 'upload') {
                    this.showModal('upload-modal', {
                        status: 'uploading',
                    });

                    const uploadParams = ['bot', '-b', this.localBot, 'upload'];
                    // TODO: handle errors
                    for await (const value of util.call(uploadParams)) {}

                    await this.showModal('upload-modal', {
                        status: 'uploaded',
                    });

                    this.closeModal();
                }
                else if (operation === 'cancel') {
                    // Canceled
                    this.closeModal();
                }
            },
        },
    }
</script>
