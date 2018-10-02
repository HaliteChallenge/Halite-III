<template>
    <div class="h-screen">
        <div id="main-logged-in" class="h-screen">
            <tab-bar :tabs="['Local Bot', 'Local Match History', loggedIn ? 'Online Bot' : 'Log In', 'Help', 'Settings']">
                <h1 slot="header" class="font-normal text-4xl ml-4">
                    <img :src="logo" alt="Halite III" class="align-bottom" />
                    <span class="text-lg italic">in a box</span>
                </h1>
                <section slot="right" class="mr-4">
                    <button class="btn btn-blue" @click="chooseReplay">Watch Replay</button>
                </section>
                <LocalBot
                    slot="tab-0"
                    :remoteBot="remoteBot"
                    :apiKey="apiKey"
                    :userId="userId"
                    @change="localBot = $event"
                />
                <MatchHistory slot="tab-1" />
                <Login
                    v-if="!loggedIn"
                    slot="tab-2"
                    @login="checkLogin"
                />
                <RemoteBot
                    v-else
                    slot="tab-2"
                    v-bind:remoteBot="remoteBot"
                    :apiKey="apiKey"
                    :userId="userId"
                />
                <!-- <LocalBotGym slot="tab-2" :localBot="localBot" /> -->
                <Settings slot="tab-4" />
            </tab-bar>
        </div>
        <component v-if="modal" :is="modal" :params="modalProps" :event="modalEvent">
        </component>
    </div>
</template>

<script>
    import { remote as electronRemote } from 'electron';

    import * as assets from './assets';
    import * as logger from './logger';
    import * as util from './util';

    import Login from './Login.vue';
    import MatchHistory from './MatchHistory.vue';
    import RemoteBot from './RemoteBot.vue';
    import LocalBot from './LocalBot.vue';
    import LocalBotGym from './LocalBotGym.vue';
    import Settings from './SettingsPage.vue';

    import logo from "../../../website/assets/images/full_logo.svg";

    export default {
        components: {
            Login,
            MatchHistory,
            RemoteBot,
            LocalBot,
            LocalBotGym,
            Settings,
        },
        data() {
            return {
                logo,
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
                logger.error(err);
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
