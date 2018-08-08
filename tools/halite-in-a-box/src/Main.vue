<template>
    <div>
        <div id="main-container" v-if="!loggedIn">
            <Login @login="checkLogin" />
        </div>
        <div v-else id="main-logged-in">
            <header id="page-header">
                <h1>Halite III <span>in a box</span></h1>
            </header>
            <RemoteBot v-bind:remoteBot="remoteBot" :apiKey="apiKey" :userId="userId" />
            <LocalBot :apiKey="apiKey" :userId="userId" />
        </div>
        <component v-if="modal" :is="modal" :params="modalProps" :event="modalEvent">
        </component>
    </div>
</template>

<script>
    import { remote as electronRemote } from 'electron';
    import * as util from './util';

    import Login from './Login.vue';
    import RemoteBot from './RemoteBot.vue';
    import LocalBot from './LocalBot.vue';

    export default {
        components: {
            Login,
            RemoteBot,
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
                for await (const value of util.call(['auth', '-m'])) {
                    this.loggedIn = value.status === "success";
                    if (this.loggedIn) {
                        this.apiKey = value.api_key;
                        this.userId = value.id;
                    }
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
        }

        #local-bot {
            grid-row: content;
            grid-column: right;
        }
    }
</style>
