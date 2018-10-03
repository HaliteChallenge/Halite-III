<template>
    <modal>
        <h2 slot="header">Compare Bot to Benchmarks</h2>

        <template slot="content">
            <template v-if="params.status === 'setup'">
                <p>
                    Test how your bot performs against benchmark bots
                    in a custom match-up. Select the bots to play
                    against and the number of games to play, and we'll
                    tell you your winrate.
                </p>
                <section class="card p-0 bg-gradient-cyan-light">
                    <section>
                        <p v-if="error" class="error">{{error}}</p>
                        <p class="italic">Select bots to play against (1 or 3)</p>
                    </section>
                    <section class="flex">
                        <div class="flex-1 border-r border-grey p-4">
                            <h3>Available Bots</h3>
                            <p class="italic">
                                Drag-and drop bots to the other side
                                to place them in the match.
                            </p>
                            <draggable v-model="availableBots" :options="{group: {name: 'bots', pull: 'clone', put: false}}" >
                                <div class="text-left bg-indigo-dark m-1 px-4 py-1 rounded" v-for="(bot, index) in availableBots" :key="index">
                                    {{bot.name}}
                                </div>
                            </draggable>
                        </div>
                        <div class="flex-1 selected-bots">
                            <h3>Selected Bots</h3>
                            <draggable
                                v-model="bots"
                                :options="{group: {name: 'bots'}}"
                            >
                                <div class="text-left bg-teal-dark m-1 px-4 py-1 rounded" v-for="(bot, index) in bots" :key="index">
                                    {{bot.name}}
                                    <button class="btn btn-blue" @click="() => bots.splice(index, 1)">Remove</button>
                                </div>
                            </draggable>
                        </div>
                    </section>
                </section>
                <section class="card card-lr mt-4">
                    <section class="card-l p-4">
                        <label for="rounds">Number of games to play:</label>
                        <input type="number" name="rounds" id="rounds" v-model="rounds" min="1" />
                    </section>
                    <section class="card-r">
                        <p>
                            Your bot will be played against the chosen bot(s) for
                            the specified number of games.
                        </p>
                    </section>
                </section>
            </template>
            <template v-else>
                <section class="card card-lr items-stretch">
                    <section class="card-l flex items-center justify-center">
                        <p>
                            {{params.message}}
                            ({{params.gamesPlayed}}/{{params.gamesTotal}} games played)
                        </p>
                    </section>
                    <section class="card-r px-4 py-2 text-left">
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
                    </section>
                </section>

                <div class="my-4 overflow-y-auto" style="max-height: 40vh;">
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
                <p>Games will also be visible in the Local Match History tab.</p>
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

<style type="css">
    .selected-bots > div {
        height: 100%;
    }
</style>
