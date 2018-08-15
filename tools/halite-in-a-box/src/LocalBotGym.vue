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
        <button>Run Games</button>

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
                        <td>{{bot.rank}} (μ={{bot.mu.toFixed(2)}}, σ={{bot.sigma.toFixed(2)}})</td>
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
        props: [],
        data() {
            return {
                bots: [],
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
        },
    }
</script>

<style lang="scss" scoped>
    #local-bot-gym {
        height: 100vh;
    }
</style>
