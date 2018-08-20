<template>
    <section id="local-bot">
        <header>
            <h2 v-if="localBot">Local Bot</h2>
            <h2 v-else>Select Local Bot</h2>
        </header>
        <template v-if="localBot">
            <p>Bot path: {{localBot}}</p>
            <p v-if="!canRunGame && botExtension === '.zip'">
                Zip files can only be uploaded, not benchmarked or bench-pressed.
            </p>
            <button @click="benchmark" :disabled="!canRunGame">Benchmark</button>
            <button @click="goToGym" :disabled="!canRunGame">Gym</button>
            <button @click="upload">Submit</button>
            <button @click="chooseLocalBot">Select Different Bot</button>
        </template>
        <template v-else>
            <p>
                Choose a bot to benchmark it against our bots,
                evaluate its performance vs your currently uploaded
                bot, or upload it to the server.
            </p>
            <button @click="chooseLocalBot">Select Bot</button>
            <button @click="goToStarterKit">Download Starter Kit</button>
        </template>
    </section>
</template>

<script>
    import { remote as electronRemote } from 'electron';
    import fs from 'fs';
    import path from 'path';
    import JSZip from 'jszip';

    import assets from './assets';
    import { pythonPath } from './assets';
    import * as util from './util';

    const RECOGNIZED_EXTENSIONS = [ '.py', '.java', '.cpp', '.js', '.zip' ];

    export default {
        props: ['apiKey', 'userId'],
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

            goToGym() {
                window.scrollTo({
                    top: document.querySelector('#local-bot-gym').offsetTop,
                    behavior: 'smooth',
                })
            },

            goToStarterKit() {
                util.openBrowserTab(`${util.WEBSITE_URL}/learn-programming-challenge/downloads-and-starter-kits/`);
            },

            async benchmark() {
                // Should have been loaded before
                const paths = await assets();

                const action = await this.showModal('benchmark-modal', {
                    status: 'setup',
                    benchmarkBots: paths.benchmarkBots,
                });
                if (action === 'cancel') {
                    this.closeModal();
                    return;
                }

                const params = ['play', '-i', action.games,
                                '-b', paths.environmentPath,
                                '--output-dir', paths.replayDir,
                                '-r', `${pythonPath()} "${this.localBot}"`];

                const stats = [{
                    won: 0,
                    name: '(your bot)',
                }];

                for (const bot of action.bots) {
                    stats.push({
                        won: 0,
                        name: bot.name,
                    });
                    params.push('-r');
                    if (bot.path === 'self') {
                        params.push(`${pythonPath()} "${this.localBot}"`);
                    }
                    else {
                        params.push(`${pythonPath()} "${bot.path}"`);
                    }
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
