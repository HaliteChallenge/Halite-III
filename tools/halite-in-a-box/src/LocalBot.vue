<template>
    <section id="local-bot" class="tab-panel">
        <section class="card flex flex-row items-center p-0">
            <section class="flex-1 border-r border-grey p-4 text-left">
                <h2 class="m-0">Choose Your Bot:</h2>
                <template v-if="localBot">
                    <p class="text-sm my-2">Selected: {{localBot}}</p>
                    <button class="btn btn-blue" @click="chooseLocalBot">Select Different Bot</button>
                </template>
                <section v-else class="my-2">
                    <button class="btn btn-blue" @click="chooseLocalBot">Select MyBot file</button>
                    <button class="btn btn-blue" @click="goToStarterKit">Download Starter Kit</button>
                </section>
                <p v-if="!canRunGame && botExtension === '.zip'">
                    Zip files can only be uploaded, not benchmarked or bench-pressed.
                </p>
            </section>

            <section class="flex-1 p-4">
                <p>
                    Load the bot you're working on, then test it
                    against our benchmark bots or the bot you last
                    submitted online. When you're confident in its
                    performance, submit it below.
                </p>
            </section>
        </section>

        <template v-if="localBot">
            <section class="flex-1 flex flex-row">
                <section class="card flex-1 mr-4 mt-4 mb-4">
                    <button class="btn btn-blue" @click="remoteBenchmark" :disabled="!canRunGame">Play Against Uploaded Bot</button>
                    <p>
                        Play your bot against the bot you currently have
                        on the server for 10 matches, to see which is
                        better.
                    </p>
                </section>
                <section class="card flex-1 ml-4 mt-4 mb-4">
                    <button class="btn btn-blue" @click="localBenchmark" :disabled="!canRunGame">Play Against Benchmark Bots</button>
                    <p>
                        Play your bot against various benchmark bots, to
                        get an idea of where you stand.
                    </p>
                </section>
            </section>

            <section
                v-if="apiKey"
                class="card flex flex-row items-center p-0"
            >
                <section class="flex-1 border-r border-grey p-0 text-left h-full">
                    <button class="btn btn-blue w-full h-full p-4 rounded-r-none" @click="upload">Upload Bot</button>
                </section>
                <section class="flex-1">
                    <p>
                        Submit your bot to halite.io and play against other players.
                    </p>
                </section>
            </section>
            <section
                v-else
                class="card"
            >
                <p>Log in at top to upload your bot.</p>
            </section>
        </template>
    </section>
</template>

<script>
    import { remote as electronRemote } from 'electron';
    import fs from 'fs';
    import path from 'path';
    import JSZip from 'jszip';

    import assets from './assets';
    import { setCwd } from './assets';
    import * as bot from './bot';
    import * as games from './games';
    import { pythonPath } from './assets';
    import * as util from './util';

    const RECOGNIZED_EXTENSIONS = [ '.py', '.java', '.cpp', '.js', '.zip' ];

    export default {
        props: ['apiKey', 'userId', 'remoteBot'],
        inject: ['assetsReady', 'showModal', 'closeModal'],
        data() {
            return {
                localBot: null,
            };
        },
        mounted() {
            if (window.localStorage.getItem('local-bot-path')) {
                this.localBot = window.localStorage.getItem('local-bot-path');
                this.$emit('change', this.localBot);
            }
        },
        computed: {
            botExtension() {
                if (this.localBot) {
                    return path.extname(this.localBot).toLowerCase();
                }
                return null;
            },
            canRunGame() {
                return this.assetsReady() && this.botExtension !== '.zip';
            },
            localBotPath() {
                if (this.localBot) {
                    // Assume bot is executable
                    let botCommand = `"${this.localBot}"`;
                    if (path.extname(this.localBot).toLowerCase() === '.py') {
                        botCommand = `"${pythonPath()}" "${this.localBot}"`;
                    }
                    // chdir to its directory first
                    return setCwd(path.dirname(this.localBot), botCommand);
                }
                return null;
            },
        },
        methods: {
            chooseLocalBot() {
                const result = electronRemote.dialog.showOpenDialog({
                    title: 'Choose bot folder or MyBot file',
                    properties: ['openFile', 'openDirectory'],
                });
                if (!result || result.length === 0) {
                    return;
                }

                const botFile = result[0];
                const extension = path.extname(botFile);

                if (RECOGNIZED_EXTENSIONS.indexOf(extension) === -1) {
                    this.showModal('local-bot-confirm-modal', {
                        localBot: botFile,
                        localBotExtension: extension,
                    }).then((action) => {
                        if (action === 'override') {
                            this.localBot = botFile;
                            this.$emit('change', this.localBot);
                        }
                    }).finally(() => this.closeModal());
                }
                else {
                    this.localBot = botFile;
                    this.$emit('change', this.localBot);
                }
            },

            goToStarterKit() {
                util.openBrowserTab(`${util.WEBSITE_URL}/learn-programming-challenge/downloads/`);
            },

            async remoteBenchmark() {
                // Should have been loaded before
                const paths = await assets();
                const params = ['play', '-i', 10,
                                '-b', paths.environmentPath,
                                '--output-dir', paths.replayDir,
                                '-r', this.localBotPath];

                const stats = [{
                    won: 0,
                    name: '(your bot)',
                }];

                const remote = new bot.RemoteBot("(currently uploaded bot)");
                stats.push({
                    won: 0,
                    name: remote.name,
                });
                params.push('-r');
                params.push(await remote.makePath());
                console.log(params);

                const state = {
                    status: 'running',
                    games: [],
                    gamesPlayed: 0,
                    gamesTotal: 10,
                    stats,
                    message: 'Running games...',
                };
                this.showModal('benchmark-modal', state);

                for await (const value of util.call(params)) {
                    console.log(value);
                    if (value.games_played) {
                        state.gamesPlayed = value.games_played;
                    }
                    if (value.stats) {
                        for (const [stringKey, gamesWon] of Object.entries(value.stats)) {
                            state.stats[parseInt(stringKey, 10)].won = gamesWon;
                        }
                    }
                    if (value.results) {
                        games.addGame(['Your Local Bot', `(your uploaded bot v${this.remoteBot.version_number})`], value.results);
                        state.games.push(value.results);
                    }
                    this.showModal('benchmark-modal', state);
                }

                state.status = 'finished';
                state.message = 'Done playing games!';
                await this.showModal('benchmark-modal', state);
                this.closeModal();
            },

            async localBenchmark() {
                // Should have been loaded before
                const paths = await assets();

                const benchmarkBots = paths.benchmarkBots.slice();
                if (this.remoteBot) {
                    benchmarkBots.push(new bot.RemoteBot(`(your uploaded bot v${this.remoteBot.version_number})`));
                }
                const action = await this.showModal('benchmark-modal', {
                    status: 'setup',
                    benchmarkBots,
                });
                if (action === 'cancel') {
                    this.closeModal();
                    return;
                }

                const params = ['play', '-i', action.games,
                                '-b', paths.environmentPath,
                                '--output-dir', paths.replayDir,
                                '-r', this.localBotPath];

                const stats = [{
                    won: 0,
                    name: '(your bot)',
                }];
                const participants = ['Your Local Bot'];

                for (const bot of action.bots) {
                    stats.push({
                        won: 0,
                        name: bot.name,
                    });
                    params.push('-r');
                    if (bot.path === 'self') {
                        params.push(this.localBotPath);
                    }
                    else {
                        params.push(await bot.makePath());
                    }
                    participants.push(bot.name);
                }

                console.log(params);

                const state = {
                    status: 'running',
                    games: [],
                    gamesPlayed: 0,
                    gamesTotal: action.games,
                    stats,
                    message: 'Running games...',
                };
                this.showModal('benchmark-modal', state);

                for await (const value of util.call(params)) {
                    console.log(value);
                    if (value.games_played) {
                        state.gamesPlayed = value.games_played;
                    }
                    if (value.stats) {
                        for (const [stringKey, gamesWon] of Object.entries(value.stats)) {
                            state.stats[parseInt(stringKey, 10)].won = gamesWon;
                        }
                    }
                    if (value.results) {
                        games.addGame(participants, value.results);
                        state.games.push(value.results);
                    }
                    this.showModal('benchmark-modal', state);
                }

                state.status = 'finished';
                state.message = 'Done playing games!';
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
        watch: {
            localBot(newValue) {
                window.localStorage.setItem('local-bot-path', newValue);
            },
        },
    }
</script>
