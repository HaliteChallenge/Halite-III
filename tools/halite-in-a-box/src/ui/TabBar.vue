<template>
    <section class="tab-bar">
        <nav>
            <button
                v-for="(tab, index) in tabs"
                @click="currentTab = index"
                :class="{ active: currentTab === index }"
            >{{tab}}</button>
        </nav>
        <section v-for="(tab, index) in tabs" :style="{ display: index === currentTab ? 'block' : 'none' }">
            <slot v-bind:name="`tab-${index}`"></slot>
        </section>
    </section>
</template>

<script>
    export default {
        name: 'tab-bar',
        props: ['tabs'],
        data() {
            return {
                currentTab: 0,
            };
        },
        mounted() {
        },
    };
</script>

<style lang="scss" scoped>
    nav {
        border-bottom: 0.1em solid #d1d1d1;
        button {
            background: none;
            color: inherit;
            border: 0;
            border-right: 0.1em solid #d1d1d1;
            border-radius: 0;
            padding: 0 1.5em;
            margin: 0;
            text-transform: none;
            font-size: inherit;
            font-weight: normal;
            font-family: inherit;

            &.active {
                font-weight: bold;
                background: #d1d1d1;
            }
        }
    }
</style>
