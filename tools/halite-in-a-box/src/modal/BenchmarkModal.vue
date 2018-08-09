<template>
    <modal>
        <h2 slot="header">Compare Bot to Benchmarks</h2>

        <template slot="content">
            <template v-if="params.status === 'setup'">
                <p v-if="error" class="error">{{error}}</p>
                <fieldset>
                    <legend>Select Bots (1 or 3)</legend>
                    <div>
                        <input type="checkbox" name="bots" id="bot-starter" value="starter" v-model="bots" />
                        <label for="bot-starter">Python Starter Bot</label>
                    </div>
                    <div>
                        <input type="checkbox" name="bots" id="bot-zerg" value="zerg" v-model="bots" />
                        <label for="bot-zerg">Zerg</label>
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
                <p>{{params.message}}</p>
                <p>{{params.gamesPlayed}}/{{params.gamesTotal}} games played</p>
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
</style>
