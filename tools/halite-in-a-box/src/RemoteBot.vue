<template>
    <section id="remote-bot" class="tab-panel">
        <template v-if="remoteBot">
            <div class="flex-initial card card-lr">
                <section class="card-l p-4">
                    <h2>Current Submission</h2>
                    <p>{{remoteBot.language}} bot version {{remoteBot.version_number}} (status: {{remoteBot.compilation_status}})</p>
                    <p>{{remoteBot.games_played}} games played</p>
                    <button class="btn btn-blue" @click="downloadBot">Download</button>
                </section>

                <section class="card-r p-4">
                    <table class="mx-auto">
                        <thead>
                            <tr>
                                <th class="px-4 py-1 border-b border-grey">Rank</th>
                                <th class="px-4 py-1 border-b border-grey" title="= μ - 3σ">Rating</th>
                                <th class="px-4 py-1 border-b border-grey">μ</th>
                                <th class="px-4 py-1 border-b border-grey">σ</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td>{{remoteBot.rank}}</td>
                                <td>{{remoteBot.score.toFixed(2)}}</td>
                                <td>{{remoteBot.mu.toFixed(2)}}</td>
                                <td>{{remoteBot.sigma.toFixed(2)}}</td>
                            </tr>
                        </tbody>
                    </table>
                    <p class="mx-auto" style="max-width: 26em">
                        Your rank is based on your rating. The rating of
                        a bot is equal to μ - 3σ.
                    </p>
                </section>
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
