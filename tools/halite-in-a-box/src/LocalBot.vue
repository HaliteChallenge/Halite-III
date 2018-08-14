<template>
    <section id="local-bot">
        <header>
            <h2 v-if="localBot">Local Bot</h2>
            <h2 v-else>Select Local Bot</h2>
        </header>
        <template v-if="localBot">
            <p>Bot path: {{localBot}}</p>
            <p v-if="message">{{message}}</p>
            <button @click="benchmark" :disabled="!assetsReady">Benchmark</button>
            <button :disabled="!assetsReady">Gym</button>
            <button @click="upload">Submit</button>
            <button @click="chooseLocalBot">Select Different Bot</button>
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

    import assets from './assets';
    import * as util from './util';

    export default {
        props: ['apiKey', 'userId'],
        inject: ['showModal', 'closeModal'],
        data() {
            return {
                localBot: null,
                assetsReady: false,
                message: 'Preparing game environment...',
            };
        },
        mounted() {
            if (window.localStorage.getItem('local-bot-path')) {
                this.localBot = window.localStorage.getItem('local-bot-path');
            }

            assets().then(() => {
                this.assetsReady = true;
                this.message = null;
            });
        },
        methods: {
            chooseLocalBot() {
                const result = electronRemote.dialog.showOpenDialog({
                    title: 'Choose bot folder or MyBot file',
                    properties: ['openFile', 'openDirectory'],
                });
                if (result && result.length > 0) {
                    this.localBot = result[0];
                }
            },

            async benchmark() {
                // TODO: loading screen
                const paths = await assets();

                const action = await this.showModal('benchmark-modal', {
                    status: 'setup',
                    benchmarkBots: paths.benchmarkBots,
                });
                console.log(action);
                if (action === 'cancel') {
                    this.closeModal();
                    return;
                }

                const params = ['gym', '-i', action.games,
                                '-b', paths.environmentPath,
                                '--output-dir', paths.replayDir,
                                '-r', 'python3 ' + this.localBot];

                const stats = [{
                    won: 0,
                    name: '(your bot)',
                }];

                // TODO: input validation
                for (const bot of action.bots) {
                    stats.push({
                        won: 0,
                        name: bot.name,
                    });
                    params.push('-r');
                    if (bot.path === 'self') {
                        params.push(`python3 "${this.localBot}"`);
                    }
                    else {
                        params.push(`python3 "${bot.path}"`);
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
