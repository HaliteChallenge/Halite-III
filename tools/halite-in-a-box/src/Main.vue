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
                <LocalBot :apiKey="apiKey" :userId="userId" @change="localBot = $event" />
                <a id="scroll-down" href="#remote-bot-history">
                    ▼Past Replays & Analytics▼
                </a>
            </div>
            <LocalBotGym :localBot="localBot" />
        </template>
        <component v-if="modal" :is="modal" :params="modalProps" :event="modalEvent">
        </component>
    </div>
</template>

<script>
    import { remote as electronRemote } from 'electron';

    import * as assets from './assets';
    import * as util from './util';

    import Login from './Login.vue';
    import RemoteBot from './RemoteBot.vue';
    import LocalBot from './LocalBot.vue';
    import LocalBotGym from './LocalBotGym.vue';

    export default {
        components: {
            Login,
            RemoteBot,
            LocalBot,
            LocalBotGym,
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
                assetsReady: false,
                assetsMessage: 'Preparing game assets...',
            };
        },
        async mounted() {
            assets.default().then(() => {
                this.assetsReady = true;
                this.assetsMessage = null;
            }).catch((err) => {
                console.error(err);
                this.assetsMessage = 'Could not set up game assets. Please try again later.';
            });

            this.checkLogin();
        },
        methods: {
            async checkLogin() {
                let loggedIn = false;
                for await (const value of util.call(['auth', '-m'])) {
                    loggedIn = value.status === "success";
                    if (loggedIn) {
                        this.apiKey = value.api_key;
                        this.userId = value.id;

                        assets.setCredentials(this.userId, this.apiKey);
                    }
                }

                // Check that API key is actually valid
                const request = await util.fetchApi(this.apiKey, 'login/me');
                const verification = await request.json();

                this.loggedIn = loggedIn && verification;

                if (this.loggedIn) {
                    this.updateRemoteBot();
                }

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
            areAssetsReady() {
                return this.assetsReady;
            },
        },
        provide() {
            return {
                showModal: this.showModal,
                closeModal: this.closeModal,
                assetsReady: this.areAssetsReady,
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
            color: inherit;
            text-decoration: none;
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
