<template>
    <section id="remote-bot-history">
        <header>
            <h2>Remote Bot Rank History</h2>
        </header>

        <p> TODO: some graphs would be nice </p>
        <div class="history-table">
            <table>
                <thead>
                    <tr>
                        <th>Bot Version</th>
                        <th>Retired On</th>
                        <th>Rank At Retirement</th>
                        <th>Score At Retirement</th>
                        <th>Games Played</th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="row in history">
                        <td>{{row.bot_version}}</td>
                        <td>{{row.when_retired}}</td>
                        <td>{{row.last_rank}}</td>
                        <td>{{row.last_score}}</td>
                        <td>{{row.last_games_played}}</td>
                    </tr>
                </tbody>
            </table>
        </div>
    </section>
</template>

<script>
    import * as util from './util';

    export default {
        props: ['userId'],
        data() {
            return {
                history: [],
            };
        },
        async mounted() {
            const request = await util.fetchApi(null, `api/user/${this.userId}/history`);
            this.history = await request.json();
        },
        methods: {
        },
    }
</script>

<style lang="scss" scoped>
    #remote-bot-history {
        height: 100vh;
        padding: 0 1em;
    }

    .history-table {
        max-height: 50vh;
        overflow-y: auto;

        th {
            background: #FFF;
            position: sticky;
            top: 0;
        }
    }
</style>
