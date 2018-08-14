<template>
    <div>
        <div id="main-container" v-if="!loggedIn">
            <Login @login="checkLogin" />
        </div>
        <template v-else>
            <div id="main-logged-in">
                <header id="page-header">
                    <h1>Halite III <span>in a box</span></h1>
                </header>
                <nav id="toolbar">
                    <button @click="chooseReplay">Watch Replay</button>
                </nav>
                <RemoteBot v-bind:remoteBot="remoteBot" :apiKey="apiKey" :userId="userId" />
                <LocalBot :apiKey="apiKey" :userId="userId" />
                <div id="scroll-down">
                    ▼Past Replays & Analytics▼
                </div>
            </div>
            <RemoteBotHistory :userId="userId" />
        </template>
        <component v-if="modal" :is="modal" :params="modalProps" :event="modalEvent">
        </component>
    </div>
</template>

<script>
    import { remote as electronRemote } from 'electron';
    import * as util from './util';

    import Login from './Login.vue';
    import RemoteBot from './RemoteBot.vue';
    import RemoteBotHistory from './RemoteBotHistory.vue';
    import LocalBot from './LocalBot.vue';

    export default {
        components: {
            Login,
            RemoteBot,
            RemoteBotHistory,
            LocalBot,
        },
        data() {
            return {
                loggedIn: false,
                apiKey: '',
                apiKeyError: '',
                remoteBot: null,
                localBot: null,
                userId: null,
                modal: null,
                modalProps: null,
                modalResolve: null,
            };
        },
        async mounted() {
            if (await this.checkLogin()) {
                this.updateRemoteBot();
            }
        },
        methods: {
            async checkLogin() {
                let loggedIn = false;
                for await (const value of util.call(['auth', '-m'])) {
                    loggedIn = value.status === "success";
                    if (loggedIn) {
                        this.apiKey = value.api_key;
                        this.userId = value.id;
                    }
                }
                const request = await util.fetchApi(this.apiKey, 'login/me');
                const verification = await request.json();

                this.loggedIn = loggedIn && verification;

                return this.loggedIn;
            },
            async updateRemoteBot() {
                const request = await util.fetchApi(this.apiKey, `api/user/${this.userId}/bot`);
                const bots = await request.json();
                if (bots && bots.length > 0) {
                    this.remoteBot = bots[0];
                }
            },
            chooseReplay() {
                const result = electronRemote.dialog.showOpenDialog({
                    title: 'Choose bot folder or MyBot file',
                    properties: ['openFile'],
                    filters: [{
                        name: 'Halite III replay',
                        extensions: ['hlt'],
                    }],
                });
                if (result && result.length > 0) {
                    util.watchReplay(result[0]);
                }
            },
            showModal(componentName, props) {
                this.modal = componentName;
                this.modalProps = props;
                return new Promise((resolve) => {
                    this.modalResolve = resolve;
                });
            },
            modalEvent(event) {
                this.modalResolve(event);
            },
            closeModal() {
                this.modal = null;
                this.modalProps = null;
            },
        },
        provide() {
            return {
                showModal: this.showModal,
                closeModal: this.closeModal,
            };
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
            [content] 1fr
            [footer] min-content;

        #page-header {
            grid-row: header;
            grid-column: left;
        }

        #toolbar {
            grid-row: header;
            grid-column: right;

            display: flex;
            justify-content: flex-end;
            align-items: center;
        }

        #scroll-down {
            grid-row: footer;
            grid-column: left / -1;
            text-align: center;
        }

        #remote-bot, #local-bot {
            align-self: center;
            padding: 1em;
            text-align: center;
        }

        #remote-bot {
            grid-row: content;
            grid-column: left;
        }

        #local-bot {
            grid-row: content;
            grid-column: right;
        }
    }
</style>
