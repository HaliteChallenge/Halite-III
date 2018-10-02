<template>
    <modal>
        <h2 slot="header">Compare Bot to Benchmarks</h2>

        <template slot="content">
            <template v-if="params.status === 'setup'">
                <p v-if="error" class="error">{{error}}</p>
                <fieldset>
                    <legend>Select Bots (1 or 3)</legend>
                    <div class="draggables">
                        <div>
                            <h3>Available Bots</h3>
                            <p>Drag-and drop bots to the other side to place them in the match.</p>
                            <draggable v-model="availableBots" :options="{group: {name: 'bots', pull: 'clone', put: false}}" >
                                <div class="draggable-item" v-for="(bot, index) in availableBots" :key="index">
                                    {{bot.name}}
                                </div>
                            </draggable>
                        </div>
                        <div>
                            <h3>Selected Bots</h3>
                            <draggable v-model="bots" :options="{group: {name: 'bots'}}" >
                                <div class="draggable-item" v-for="(bot, index) in bots" :key="index">
                                    {{bot.name}}
                                    <button class="btn btn-blue" @click="() => bots.splice(index, 1)">Remove</button>
                                </div>
                            </draggable>
                        </div>
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
                <p style="margin: 0;">{{params.message}} ({{params.gamesPlayed}}/{{params.gamesTotal}} games played)</p>
                <table>
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
                <button class="btn btn-blue" @click="event('cancel')">Cancel</button>
                <button class="btn btn-blue" @click="start">Start</button>
            </template>
            <template v-else-if="params.status === 'finished'">
                <button class="btn btn-blue" @click="event()">Done</button>
            </template>
        </template>
    </modal>
</template>

<script>
    import { shell } from 'electron';
    import draggable from 'vuedraggable'

    import * as util from '../util';

    export default {
        props: ['event', 'params'],
        components: { draggable },
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
        computed: {
            availableBots() {
                const result = [];
                for (const bot of this.params.benchmarkBots) {
                    result.push(bot);
                }
                result.push({
                    name: '(a copy of your bot)',
                    path: 'self',
                });
                return result;
            },
        },
    }
</script>

<style lang="scss" scoped>
    label {
        display: inline;
        font-weight: normal;
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

    .draggables {
        display: flex;

        p {
            margin: 1rem 0;
        }

        .draggable-item {
            border: 1px solid #d1d1d1;
            border-radius: 5px;
            margin: 0.5em 0;
            cursor: move;
            position: relative;
        }

        >div {
            flex: 1;

            h3 {
                font-size: 1em;
                font-style: italic;
                margin: 0;
            }

            >div {
                /* Draggable container */
                height: 100%;

                button {
                    height: auto;
                    padding: 0.25em 2em;
                    line-height: 1.5em;
                    margin: 0;
                    position: absolute;
                    top: 0;
                    right: 0;
                    bottom: 0;
                }
            }
        }

        >div:first-child {
            margin-right: 0.5em;
        }

        >div:last-child {
            border-left: 0.1rem solid #d1d1d1;
            padding-left: 0.5em;
        }
    }
</style>
