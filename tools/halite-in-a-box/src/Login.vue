<template>
    <section class="tab-panel">
        <div class="card text-left">
            <p>
                Log in with your API key to upload bots and check the
                status of your online bot.
            </p>

            <ol>
                <li><a href="#" @click="openProfile">Click here to open your settings page on the website.</a></li>
                <li>Click "Generate API key".</li>
                <li>Copy and paste the result below.</li>
            </ol>

            <form class="text-center mt-4">
                <label for="api-key">Paste API key:</label>
                <input type="text" id="api-key" v-model="apiKey" class="font-mono" />
                <p class="error" v-if="apiKeyError">{{apiKeyError}}</p>
            </form>
        </div>
    </section>
</template>

<script>
    import * as util from './util';

    export default {
        data() {
            return {
                apiKey: '',
                apiKeyError: '',
            };
        },

        methods: {
            openProfile() {
                util.openBrowserTab(`${util.WEBSITE_URL}/user/settings`);
            },
        },
        watch: {
            async apiKey(newValue) {
                this.apiKeyError = '';
                if (newValue && newValue.indexOf(':') === -1) {
                    this.apiKeyError = 'API key is invalid.';
                }
                else {
                    this.apiKeyError = 'Checking API key...';
                    for await (const value of util.call(['auth', '-k', newValue])) {
                        if (value.status === 'error') {
                            this.apiKeyError = value.message;
                        }
                        else {
                            this.apiKeyError = '';
                        }
                    }
                    if (!this.apiKeyError) {
                        this.$emit('login');
                    }
                }
            },
        },
    }
</script>

<style lang="scss" scoped>
    #main {
        display: flex;
        justify-content: center;
        height: 100vh;
    }
</style>
