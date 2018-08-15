<template>
    <section id="local-bot-gym">
        <header>
            <h2>Local Bot Gym</h2>
        </header>

        <p> TODO: some graphs would be nice </p>
        <div class="gym-bots">
            <table>
                <thead>
                    <tr>
                        <th>Name</th>
                        <th>Bot Version</th>
                        <th>Rank</th>
                        <th>Path to Bot</th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="bot in bots">
                        <td>{{bot.name}}</td>
                        <td>{{bot.version}}</td>
                        <td>{{bot.rank}} (μ={{bot.mu.toFixed(2)}}, σ={{bot.sigma.toFixed(2)}})</td>
                        <td>{{bot.path}}</td>
                    </tr>
                </tbody>
            </table>
        </div>
    </section>
</template>

<script>
    import * as util from './util';

    export default {
        props: [],
        data() {
            return {
                bots: [],
            };
        },
        async mounted() {
            for await (const value of util.call(['gym', 'bots'])) {
                this.bots = value.items;
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
