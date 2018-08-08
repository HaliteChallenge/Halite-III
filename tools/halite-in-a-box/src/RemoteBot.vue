<template>
    <section id="remote-bot">
        <header>
            <h2>Current Bot</h2>
        </header>

        <template v-if="remoteBot">
            <p>{{remoteBot.language}} bot version {{remoteBot.version_number}} (status: {{remoteBot.compilation_status}})</p>
            <p>Rank {{remoteBot.rank}}</p>
            <p>{{remoteBot.games_played}} games played</p>

            <table>
                <thead>
                    <tr>
                        <th>μ</th> <th>σ</th> <th title="= μ - 3σ">Score</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>{{remoteBot.mu.toFixed(2)}}</td>
                        <td>{{remoteBot.sigma.toFixed(2)}}</td>
                        <td>{{remoteBot.score.toFixed(2)}}</td>
                    </tr>
                </tbody>
            </table>

            <button v-on:click="downloadBot">Download</button>
        </template>

        <p v-else>
            No bot submitted. Select a bot on the right.
        </p>
    </section>
</template>

<script>
    import { remote as electronRemote } from 'electron';
    import fs from 'fs';

    export default {
        props: ['apiKey', 'userId', 'remoteBot'],
        data() {
            return {};
        },
        methods: {
             async downloadBot() {
                const request = await util.fetchApi(
                    this.apiKey,
                    `api/user/${this.userId}/bot/0`,
                    {
                        headers: {
                            'Accept': 'application/zip',
                        },
                    });
                const botZip = await request.arrayBuffer();
                const path = electronRemote.dialog.showSaveDialog({
                    title: 'Choose path to save bot',
                    filters: [{
                        name: 'ZIP file',
                        extensions: ['zip'],
                    }],
                });
                fs.writeFileSync(path, new Uint8Array(botZip));
            },
        },
    }
</script>

<style lang="scss">
    table {
        margin: 0 auto 2em;
        width: auto;

        th {
            text-align: center;
            padding: 0;
        }

        td {
            text-align: center;
            padding: 0 1em;
            font-size: 1.5em;
        }
    }
</style>
