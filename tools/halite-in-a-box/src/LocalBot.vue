<template>
    <section id="local-bot">
        <header>
            <h2>Select Local Bot</h2>
        </header>
        <template v-if="localBot">
            <p>Bot path: {{localBot}}</p>
            <button>Benchmark</button>
            <button>Gym</button>
            <button>Upload</button>
        </template>
        <template v-else>
            <p>
                Choose a bot to benchmark it against our bots,
                evaluate its performance vs your currently uploaded
                bot, or upload it to the server.
            </p>
            <button v-on:click="chooseLocalBot">Select Bot</button>
        </template>
    </section>
</template>

<script>
    import { remote as electronRemote } from 'electron';

    import * as util from './util';

    export default {
        props: ['apiKey', 'userId'],
        data() {
            return {
                localBot: null,
            };
        },
        methods: {
            chooseLocalBot() {
                const [ path ] = electronRemote.dialog.showOpenDialog({
                    title: 'Choose bot folder or MyBot file',
                    properties: ['openFile', 'openDirectory'],
                });
                this.localBot = path;
            },
        },
    }
</script>
