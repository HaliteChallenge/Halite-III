<template>
    <section id="local-bot">
        <header>
            <h2>Select Local Bot</h2>
        </header>
        <template v-if="localBot">
            <p>Bot path: {{localBot}}</p>
            <button>Benchmark</button>
            <button>Gym</button>
            <button @click="upload">Upload</button>
        </template>
        <template v-else>
            <p>
                Choose a bot to benchmark it against our bots,
                evaluate its performance vs your currently uploaded
                bot, or upload it to the server.
            </p>
            <button v-on:click="chooseLocalBot">Select Bot</button>
        </template>
    </section>
</template>

<script>
    import { remote as electronRemote } from 'electron';

    import * as util from './util';

    export default {
        props: ['apiKey', 'userId'],
        inject: ['showModal', 'closeModal'],
        data() {
            return {
                localBot: null,
            };
        },
        methods: {
            chooseLocalBot() {
                const [ path ] = electronRemote.dialog.showOpenDialog({
                    title: 'Choose bot folder or MyBot file',
                    properties: ['openFile', 'openDirectory'],
                });
                this.localBot = path;
            },
            async upload() {
                let items = null;
                const params = ['bot', '-b', this.localBot, 'upload', '--dry-run'];
                for await (const value of util.call(params)) {
                    if (value.items) {
                        items = value.items;
                    }
                }
                const operation = await this.showModal('upload-modal', {
                    status: 'previewing',
                    localBot: this.localBot,
                    files: items,
                });

                if (operation === 'upload') {
                    this.showModal('upload-modal', {
                        status: 'uploading',
                    });

                    const uploadParams = ['bot', '-b', this.localBot, 'upload'];
                    // TODO: handle errors
                    for await (const value of util.call(uploadParams)) {}

                    await this.showModal('upload-modal', {
                        status: 'uploaded',
                    });

                    this.closeModal();
                }
                else if (operation === 'cancel') {
                    // Canceled
                    this.closeModal();
                }
            },
        },
    }
</script>
