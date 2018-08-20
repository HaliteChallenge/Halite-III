<template>
    <div id="main">
        <div>
            <h1>Halite III <span>in a box</span></h1>

            <div>
                <ol>
                    <li><a href="#" @click="openProfile">Click here to open your settings page on the website.</a></li>
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
                util.openBrowserTab('http://35.241.33.112/user/settings');
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
