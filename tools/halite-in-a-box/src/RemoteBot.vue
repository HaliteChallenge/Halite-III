<template>
    <section id="remote-bot" class="tab-panel">
        <template v-if="remoteBot">
            <div class="flex-initial card">
                <h2>Current Submission</h2>
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

                <button class="btn btn-blue" @click="downloadBot">Download</button>
            </div>
        </template>

        <p v-else>
            No bot uploaded. Select a bot on the right to upload it.
        </p>
    </section>
</template>

<script>
    import { remote as electronRemote } from 'electron';
    import fs from 'fs';
    import * as util from './util';

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
                if (path) {
                    fs.writeFileSync(path, new Uint8Array(botZip));
                }
            },
        },
    }
</script>

<style lang="scss" scoped>
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
