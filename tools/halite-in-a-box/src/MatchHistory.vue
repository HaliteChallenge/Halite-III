<template>
    <section class="tab-panel flex flex-col">
        <h2>Local Match History</h2>
        <section class="overflow-auto">
            <table class="w-full text-left border-collapse">
                <thead>
                    <tr>
                        <th>#</th>
                        <th>Date Played</th>
                        <th>Map Size</th>
                        <th>Participants</th>
                        <th></th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="game in games">
                        <td>{{game.id}}</td>
                        <td>{{game.date_played}}</td>
                        <td>{{game.map_width}}x{{game.map_height}}</td>
                        <td>
                            <ul>
                                <li v-for="(participant, index) in game.participants">
                                    {{participant}} (rank #{{game.stats[index].rank}})

                                    <a v-if="game.error_logs && game.error_logs[index]"
                                       href="javascript: void 0;"
                                       @click="showLog(game.error_logs[index])">
                                        (log file)
                                    </a>
                                </li>
                            </ul>
                        </td>
                        <td><a href="javascript: void 0;" @click="watchReplay(game.replay)">Watch Replay</a></td>
                    </tr>
                </tbody>
            </table>
        </section>
    </section>
</template>

<script>
    import { shell } from 'electron';
    import * as games from './games';
    import * as util from './util';

    export default {
        data() {
            return {
                games: [],
            };
        },
        methods: {
            /**
             * Called by tab component when this tab is
             * opened. Refreshes games list.
             */
            tabOpened() {
                games.listGames().then((games) => {
                    this.games = games;
                })
            },
            showLog(path) {
                shell.openItem(path);
            },
            watchReplay(path) {
                util.watchReplay(path);
            },
        },
    };
</script>

<style lang="scss" scoped>
    .local-match-history {
        padding: 1em;
    }

    td ul {
        margin: 0;

        li {
            margin: 0;
        }
    }
</style>
