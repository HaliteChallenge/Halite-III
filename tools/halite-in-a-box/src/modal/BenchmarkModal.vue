<template>
    <modal>
        <h1 slot="header">Compare Bot to Benchmarks</h1>

        <template slot="content">
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
            </fieldset>
            <label for="rounds">Number of games to play:</label>
            <input type="number" name="rounds" id="rounds" v-model="rounds" min="1" />
        </template>

        <template slot="buttons">
            <button @click="props.event('cancel')">Cancel</button>
            <button @click="start">Start</button>
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
                this.event({
                    action: 'start',
                    games: this.rounds,
                    bots: this.bots,
                });
            },
        },
    }
</script>
