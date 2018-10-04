<template>
    <section id="remote-bot" class="tab-panel overflow-y-auto">
        <template v-if="remoteBot">
            <div class="flex-initial flex-no-shrink card card-lr">
                <section class="card-l p-4">
                    <h2>Current Submission</h2>
                    <p>{{remoteBot.language}} bot version {{remoteBot.version_number}} (status: {{remoteBot.compilation_status}})</p>
                    <p>{{remoteBot.games_played}} games played</p>
                    <button class="btn btn-blue" @click="downloadBot">Download</button>
                </section>

                <section class="flex-1 p-4 border-r border-grey">
                    <h3>Rank {{remoteBot.rank}}</h3>
                    <p v-if="leaderboard">{{leaderboard.tier}} tier</p>
                    <p v-if="leaderboard && leaderboard.organization_rank">
                        {{leaderboard.organization}} player rank {{leaderboard.organization_rank}}
                    </p>
                </section>

                <section class="card-r p-4">
                    <table class="mx-auto text-center">
                        <thead>
                            <tr>
                                <th class="px-4 py-1 border-b border-grey text-center" title="= μ - 3σ">Rating</th>
                                <th class="px-4 py-1 border-b border-grey text-center">μ</th>
                                <th class="px-4 py-1 border-b border-grey text-center">σ</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
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

            <section v-if="history.length > 0" class="card mt-4 flex-no-shrink">
                <h3>Bot Submission History</h3>

                <table>
                    <thead>
                        <tr>
                            <th>Version</th>
                            <th>Games Played</th>
                            <th>Rank</th>
                            <th>Score</th>
                            <th>Retired On</th>
                        </tr>
                    </thead>

                    <tbody>
                        <tr v-for="bot in history" :key="bot.bot_version">
                            <td class="text-left">{{bot.bot_version}}</td>
                            <td class="text-left">{{bot.last_games_played}}</td>
                            <td class="text-left">{{bot.last_rank}}</td>
                            <td class="text-left">{{bot.last_score}}</td>
                            <td class="text-left">{{new Date(bot.when_retired).toLocaleString()}}</td>
                        </tr>
                    </tbody>
                </table>
            </section>
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
            return {
                leaderboard: null,
                history: [],
            };
        },
        async mounted() {
            const request = await util.fetchApi(
                this.apiKey,
                `api/leaderboard?filter=user_id,=,${this.userId}`);
            this.leaderboard = (await request.json())[0];

            const r2 = await util.fetchApi(
                this.apiKey,
                `api/user/${this.userId}/history`);
            this.history = await r2.json();
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
