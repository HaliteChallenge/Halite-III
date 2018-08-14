<template>
    <modal>
        <h2 slot="header">Compare Bot to Benchmarks</h2>

        <template slot="content">
            <template v-if="params.status === 'setup'">
                <p v-if="error" class="error">{{error}}</p>
                <fieldset>
                    <!-- TODO: drag-and-drop bots from left to right so you can multiple copies of a bot -->
                    <legend>Select Bots (1 or 3)</legend>
                    <div v-for="(bot, index) in params.benchmarkBots">
                        <input
                            type="checkbox"
                            name="bots"
                            :id="`bot-${index}`"
                            :value="bot"
                            v-model="bots"
                        />
                        <label :for="`bot-${index}`">{{bot.name}}</label>
                    </div>
                    <div>
                        <input type="checkbox" name="bots" id="bot-self" value="self" v-model="bots" />
                        <label for="bot-self">(against itself)</label>
                    </div>
                </fieldset>
                <label for="rounds">Number of games to play:</label>
                <input type="number" name="rounds" id="rounds" v-model="rounds" min="1" />
                <p>
                    Your bot will be played against the chosen bot(s) for
                    the specified number of games.
                </p>
            </template>
            <template v-else>
                <p>{{params.message}} ({{params.gamesPlayed}}/{{params.gamesTotal}} games played)</p>
                <table>
                    <caption>Game Results Summary</caption>
                    <thead>
                        <tr>
                            <th>Bot</th>
                            <th>Games Won</th>
                            <th>Win %</th>
                        </tr>
                    </thead>
                    <tbody>
                        <tr v-for="bot in params.stats">
                            <td>{{bot.name}}</td>
                            <td>{{bot.won}}</td>
                            <td>{{(100 * bot.won / params.gamesTotal).toFixed(1)}}%</td>
                        </tr>
                    </tbody>
                </table>

                <div class="scroll-table">
                    <table>
                        <caption>Individual Games</caption>
                        <thead>
                            <tr>
                                <th>Game #</th>
                                <th>Map Parameters</th>
                                <th>Your Bot Rank</th>
                                <th></th>
                            </tr>
                        </thead>

                        <tbody>
                            <tr v-for="(game, index) in params.games">
                                <td>{{index + 1}}</td>
                                <td>{{game.map_width}}×{{game.map_height}}, seed: {{game.map_seed}}</td>
                                <td>
                                    #{{game.stats[0].rank}}
                                    <template v-if="game.terminated && game.terminated[0]">
                                        (crashed)
                                    </template>
                                </td>
                                <td>
                                    <a href="javascript: void 0;" @click="showReplay(game.replay)">Watch Replay</a>
                                    <a v-if="game.error_logs && game.error_logs[0]"
                                       href="javascript: void 0;"
                                       @click="showErrorLog(game.error_logs[0])">
                                        View Log
                                    </a>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </template>
        </template>

        <template slot="buttons">
            <template v-if="params.status === 'setup'">
                <button @click="event('cancel')">Cancel</button>
                <button @click="start">Start</button>
            </template>
            <template v-else-if="params.status === 'finished'">
                <button @click="event()">Done</button>
            </template>
        </template>
    </modal>
</template>

<script>
    import { shell } from 'electron';
    import * as util from '../util';

    export default {
        props: ['event', 'params'],
        data() {
            return {
                rounds: 10,
                bots: [],
                error: null,
            };
        },
        methods: {
            start() {
                if (this.bots.length !== 1 && this.bots.length !== 3) {
                    this.error = 'Must pick 1 or 3 bots.';
                    return;
                }
                this.event({
                    action: 'start',
                    games: this.rounds,
                    bots: this.bots,
                });
            },
            showReplay(path) {
                util.watchReplay(path);
            },
            showErrorLog(path) {
                shell.openItem(path);
            },
        },
    }
</script>

<style lang="scss" scoped>
    label {
        display: inline;
        font-weight: normal;
    }

    fieldset {
        border: 0.1rem solid #d1d1d1;
        border-radius: 5px;
        padding: 0 1em;
    }

    .scroll-table {
        max-height: 30vh;
        overflow-y: scroll;

        caption {
            position: sticky;
            top: 0;
            height: 1.5em;
            background: #FFF;
        }

        thead th {
            position: sticky;
            top: 1.5em;
            background: #FFF;
            background-clip: padding-box;
        }
    }
</style>
