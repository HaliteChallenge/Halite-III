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

        <div class="bot-history" v-if="botHistory && botHistory.length > 0">
            <table>
                <caption>Your Bot's Performance History</caption>
                <thead>
                    <tr>
                        <th>Timestamp</th>
                        <th>Rank</th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="record in botHistory">
                        <td>{{record.datetime}}</td>
                        <td>{{record.rank}} (score={{(record.mu - 3*record.sigma).toFixed(2)}}, μ={{record.mu.toFixed(2)}}, σ={{record.sigma.toFixed(2)}})</td>
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

    export default {
        props: ['localBot'],
        data() {
            return {
                bots: [],
                botHistory: [],
                message: null,
                iterations: 10,

                registeringBots: false,
            };
        },
        mounted() {
        },
        methods: {
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
                        let found = false;
                        for (const registeredBot of this.bots) {
                            if (bot.name === registeredBot.name) {
                                found = true;
                                break;
                            }
                        }

                        // Don't re-register bots each time
                        if (found) continue;

                        const args = ['gym', 'register', bot.name, `${pythonPath()} "${bot.path}"`];
                        for await (const _ of util.call(args)) {
                            ;
                        }
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
                            for await (const _ of util.call(args)) {
                                ;
                            }
                        }
                    }

                    await this.refresh();
                }
                finally {
                    this.registeringBots = false;
                }
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
    }
</style>
