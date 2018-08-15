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
        <button @click="runGymGames">Run Games</button>
        <p v-if="message">{{message}}</p>
        <label for="iterations">Number of games:</label>
        <input type="number" min="1" step="1" id="iterations" v-model="iterations" />

        <p> TODO: some graphs would be nice </p>
        <div class="gym-bots">
            <table>
                <thead>
                    <tr>
                        <th>Name</th>
                        <th>Bot Version</th>
                        <th>Rank</th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="bot in bots">
                        <td>{{bot.name}}</td>
                        <td>{{bot.version}}</td>
                        <td>{{bot.rank}} (score={{(bot.mu - 3*bot.sigma).toFixed(2)}}, μ={{bot.mu.toFixed(2)}}, σ={{bot.sigma.toFixed(2)}})</td>
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
                message: null,
                iterations: 10,
            };
        },
        async mounted() {
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
            },
        },
        watch: {
            async localBot(path) {
                if (!path) return;

                // Make sure we have current bot list
                let bots = [];
                for await (const value of util.call(['gym', 'bots'])) {
                    bots = value.items;
                }

                // Register player bot if necessary
                const fullPath = `${pythonPath()} "${path}"`;
                let found = false;
                for (const registeredBot of bots) {
                    if (registeredBot.name === "My Bot") {
                        found = registeredBot.path === fullPath;
                    }
                }

                if (!found) {
                    const args = ['gym', 'register', "My Bot", fullPath];
                    for await (const _ of util.call(args)) {
                        ;
                    }

                    await this.refresh();
                }
            },
        },
    }
</script>

<style lang="scss" scoped>
    #local-bot-gym {
        height: 100vh;
    }
</style>
