<template>
    <modal>
        <h1 slot="header">Compare Bot to Benchmarks</h1>

        <template slot="content">
            <template v-if="params.status === 'setup'">
                <fieldset>
                    <legend>Select Bots</legend>
                    <div>
                        <input type="checkbox" name="bots" id="bot-starter" value="starter" v-model="bots" />
                        <label for="bot-starter">Starter Bot</label>
                    </div>
                    <div>
                        <input type="checkbox" name="bots" id="bot-zerg" value="zerg" v-model="bots" />
                        <label for="bot-zerg">Zerg</label>
                    </div>
                    <div>
                        <input type="checkbox" name="bots" id="bot-self" value="self" v-model="bots" />
                        <label for="bot-zerg">(against itself)</label>
                    </div>
                </fieldset>
                <label for="rounds">Number of games to play:</label>
                <input type="number" name="rounds" id="rounds" v-model="rounds" min="1" />
            </template>
            <template v-else>
                <p>{{params.gamesPlayed}}/{{params.gamesTotal}} games played</p>
                <p>Win ratio: {{params.stats}}</p>
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
            };
        },
        methods: {
            start() {
                console.log(this.rounds, this.bots);
                this.event({
                    action: 'start',
                    games: this.rounds,
                    bots: this.bots,
                });
            },
        },
    }
</script>
