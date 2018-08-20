<template>
    <section id="local-bot-gym">
        <header>
            <h2>Local Bot Gym</h2>
        </header>

        <p>
            Your own personal tournament of your bot vs. our
            benchmarks. As games are run, your bot is ranked against
            ours. Track your progress over time.
        </p>

        <fieldset>
            <legend>Run Ranking Games</legend>
            <label for="iterations">Number of games:</label>
            <input type="number" min="1" step="1" id="iterations" v-model="iterations" />
            <button @click="runGymGames">Start</button>
            <p v-if="message">{{message}}</p>
        </fieldset>

        <p> TODO: some graphs would be nice </p>
        <div class="gym-bots">
            <table>
                <thead>
                    <tr>
                        <th>Name</th>
                        <th>Bot Version</th>
                        <th>Rank</th>
                        <th># Games Played</th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="bot in bots">
                        <td>{{bot.name}}</td>
                        <td>{{bot.version}}</td>
                        <td>{{bot.rank}} (score={{(bot.mu - 3*bot.sigma).toFixed(2)}}, μ={{bot.mu.toFixed(2)}}, σ={{bot.sigma.toFixed(2)}})</td>
                        <td>{{bot.games_played}}</td>
                    </tr>
                </tbody>
            </table>
        </div>

        <section class="graphs">
            <section class="graph">
                <h3>Bot score (=μ-3σ) over time</h3>
                <history-chart
                    v-if="botHistory && botHistory.length > 0"
                    :invertY="false"
                    :startYZero="true"
                    width="400" height="320"
                    :lines="[[botHistory, drawScore]]" />
            </section>
            <section class="graph">
                <h3>Bot rank over time</h3>
                <history-chart
                    v-if="botHistory && botHistory.length > 0"
                    :invertY="true"
                    :startYZero="false"
                    width="400" height="320"
                    :lines="[[botHistory, drawRank]]" />
            </section>
        </section>

        <div class="gym-bots">
            <table>
                <caption>Games</caption>
                <thead>
                    <tr>
                        <th>Date Played</th>
                        <th>Participants</th>
                        <th>Map Parameters</th>
                        <th></th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="game in games">
                        <td>{{game.datetime}}</td>
                        <td>
                            <span v-for="participant in matchParticipants(game)">
                                "{{participant.name}}" (rank {{participant.rank}})
                            </span>
                        </td>
                        <td>{{game.results.map_width}}x{{game.results.map_height}}, seed: {{game.results.map_seed}}</td>
                        <td>
                            <a @click="watchReplay(game.results.replay)">Watch Replay</a>
                            View Log
                        </td>
                    </tr>
                </tbody>
            </table>
        </div>
    </section>
</template>

<script>
    import assets from './assets';
    import { pythonPath } from './assets';
    import * as util from './util';

    import LocalBotHistoryChart from './LocalBotHistoryChart.vue';

    export default {
        props: ['localBot'],
        components: {
            "history-chart": LocalBotHistoryChart,
        },
        data() {
            return {
                bots: [],
                games: [],
                botHistory: [],
                message: null,
                iterations: 10,

                registeringBots: false,
            };
        },
        mounted() {
        },
        methods: {
            drawRank(d) {
                return d.rank;
            },

            drawScore(d) {
                return d.mu - 3*d.sigma;
            },

            matchParticipants(game) {
                const participants = game.participants.slice();
                for (const [ key, stats ] of Object.entries(game.results.stats)) {
                    participants[stats.rank - 1] = game.participants[key];
                    participants[stats.rank - 1].rank = stats.rank;
                }
                return participants;
            },

            async runGymGames() {
                this.message = 'Running games...';

                const paths = await assets();
                const args = [
                    'gym', 'evaluate',
                    '-b', paths.environmentPath,
                    '-i', this.iterations,
                    '--output-dir', paths.replayDir,
                ];

                for await (const value of util.call(args)) {
                    this.message = value.message;
                    await this.refresh();
                }
            },

            async refresh() {
                for await (const value of util.call(['gym', 'bots'])) {
                    this.bots = value.items;
                }

                for await (const value of util.call(['gym', 'stats'])) {
                    this.games = value.items;
                }

                // Get bot score history
                for await (const value of util.call(['gym', 'bots', 'My Bot'])) {
                    if (value.status === 'success') {
                        this.botHistory = value.history;
                    }
                }
            },

            async registerBots() {
                if (this.registeringBots) return;

                this.registeringBots = true;

                try {
                    const { benchmarkBots } = await assets();
                    for await (const value of util.call(['gym', 'bots'])) {
                        this.bots = value.items;
                    }

                    for (const bot of benchmarkBots) {
                        // Don't register the online bot
                        if (!bot.local) {
                            continue;
                        }

                        let found = false;
                        for (const registeredBot of this.bots) {
                            if (bot.name === registeredBot.name) {
                                found = true;
                                break;
                            }
                        }

                        // Don't re-register bots each time
                        if (found) continue;

                        const args = ['gym', 'register', bot.name, await bot.makePath()];
                        console.info('Registering bot with args', args);
                        await util.collectCall(args);
                    }

                    // Register player bot if necessary
                    if (this.localBot) {
                        const fullPath = `${pythonPath()} "${this.localBot}"`;
                        let found = false;
                        for (const registeredBot of this.bots) {
                            if (registeredBot.name === "My Bot") {
                                found = registeredBot.path === fullPath;
                            }
                        }

                        if (!found) {
                            const args = ['gym', 'register', "My Bot", fullPath];
                            await util.collectCall(args);
                        }
                    }

                    await this.refresh();
                }
                finally {
                    this.registeringBots = false;
                }
            },

            watchReplay(path) {
                util.watchReplay(path);
            },
        },
        watch: {
            localBot(path) {
                if (!path) return;

                this.registerBots();
            },
        },
    }
</script>

<style lang="scss" scoped>
    #local-bot-gym {
        min-height: 100vh;
        padding: 0 1em;

        header {
            background: #FFF;
            position: sticky;
            top: 0;
        }

        fieldset {
            label, input {
                display: inline-block;
                width: auto;
            }
        }

        .graphs {
            display: flex;
            flex-wrap: wrap;
            justify-content: space-evenly;

            .graph {
                flex: 1 0 400px;

                h3 {
                    text-align: center;
                }

                svg {
                    display: block;
                    margin: 0 auto;
                }
            }
        }
    }
</style>
