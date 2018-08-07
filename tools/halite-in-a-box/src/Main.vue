<template>
    <div id="main-container" v-if="!loggedIn">
        <div id="main">
            <h1>Halite III <span>in a box</span></h1>

            <div>
                <button v-on:click="openProfile">Log In</button>
                <ol>
                    <li>Go to your profile on the website.</li>
                    <li>Click "Generate API key".</li>
                    <li>Copy and paste the result below.</li>
                </ol>
            </div>

            <form>
                <label for="api-key">Paste API key:</label>
                <input type="text" id="api-key" v-model="apiKey" />
                <p class="error" v-if="apiKeyError">{{apiKeyError}}</p>
            </form>
        </div>
    </div>
    <div v-else id="main-logged-in">
        <header id="page-header">
            <h1>Halite III <span>in a box</span></h1>
        </header>
        <section id="remote-bot">
            <header>
                <h2>Current Bot</h2>
            </header>

            <template v-if="remoteBot">
                <p>{{remoteBot.language}} bot version {{remoteBot.version_number}} (status: {{remoteBot.compilation_status}})</p>
                <p>Rank {{remoteBot.rank}}</p>

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
        <section id="local-bot">
            <header>
                <h2>Select Local Bot</h2>
            </header>
            <p>
                Choose a bot to benchmark it against our bots,
                evaluate its performance vs your currently uploaded
                bot, or upload it to the server.
            </p>
            <label for="local-bot-file" id="local-bot-file-label">Select Bot</label>
            <input type="file" id="local-bot-file">
            <p>Choose a previously selected bot:</p>
        </section>
    </div>
</template>

<script>
    import { remote as electronRemote } from 'electron';
    import fs from 'fs';
    import * as util from './util';

    export default {
        data() {
            return {
                loggedIn: false,
                apiKey: '',
                apiKeyError: '',
                remoteBot: null,
                userId: null,
            };
        },
        async mounted() {
            if (await this.checkLogin()) {
                this.updateRemoteBot();
            }
        },
        methods: {
            async checkLogin() {
                for await (const value of util.call(['auth', '-m'])) {
                    this.loggedIn = value.status === "success";
                    if (this.loggedIn) {
                        this.apiKey = value.api_key;
                        this.userId = value.id;
                    }
                }
                return this.loggedIn;
            },
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
            openProfile() {
                util.openBrowserTab('http://35.241.33.112/user/settings');
            },
            async updateRemoteBot() {
                const request = await util.fetchApi(this.apiKey, `api/user/${this.userId}/bot`);
                const bots = await request.json();
                if (bots && bots.length > 0) {
                    this.remoteBot = bots[0];
                }
            },
        },
        watch: {
            async apiKey(newValue) {
                this.apiKeyError = '';
                if (newValue && newValue.indexOf(':') === -1) {
                    this.apiKeyError = 'API key is invalid.';
                }
                else {
                    for await (const value of util.call(['auth', '-k', newValue])) {
                        if (value.status === 'error') {
                            this.apiKeyError = value.message;
                        }
                    }
                    if (!this.apiKeyError) {
                        this.checkLogin();
                    }
                }
            }
        },
    };
</script>

<style lang="scss" scoped>
    #main-container {
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;

        width: 100%;
        height: 100%;
    }

    #main {
        display: flex;
        flex-direction: column;

        text-align: center;

        ol {
            text-align: left;
        }
    }

    h1 {
        span {
            font-size: 0.6em;
        }
    }

    #main-logged-in {
        display: grid;
        box-sizing: border-box;
        width: 100vw;
        height: 100vh;

        grid-template-columns:
            [left] 1fr
            [right] 1fr;

        grid-template-rows:
            [header] 4em
            [content] 1fr;

        #page-header {
            grid-row: header;
            grid-column: left / -1;
        }

        #remote-bot, #local-bot {
            align-self: center;
            padding: 1em;
            text-align: center;
        }

        #remote-bot {
            grid-row: content;
            grid-column: left;

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
        }

        #local-bot {
            grid-row: content;
            grid-column: right;
        }
    }
</style>
