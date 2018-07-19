<template>
    <section>
        <Visualizer
            v-if="replay"
            v-bind:replay="replay"
            v-bind:game="game"
            :dashboard="false"
            :width="500"
            :height="500"
            :autoplay="false"
        />
    </section>
</template>

<script>
    import * as libhaliteviz from '../../../libhaliteviz'
    import Visualizer from "./Visualizer.vue";

    export default {
        name: "beginner-tutorial-stepper",
        props: ["progress", "step-name"],
        inject: ["completeSubstep"],
        components: {Visualizer},
        data: function () {
            return {
                replay: null,
                game: null,
            };
        },
        mounted: function () {
            window.fetch("/assets/replay.hlt")
                  .then((req) => req.arrayBuffer())
                  .then((buf) => libhaliteviz.parseReplay(buf))
                  .then((replay) => {
                      this.replay = replay;
                  });
        },
        methods: {

        },
    };
</script>

<style lang="scss" scoped>

</style>

<style lang="scss">
    .sidebar, .game-heading, .xline {
        display: none;
    }

    .game-replay, .game-replay-viewer, .visuallizer-container {
        padding: 0 !important;
    }

    .visuallizer-container {
        height: 100%;
    }

    .visuallizer-container > .row {
        display: flex;
        justify-content: center;
        align-items: center;
        height: 100%;
    }

    .visuallizer-container > .row > .col-md-8 {
        width: 100%;
        flex: 1 0;
    }

    .walkthrough-content {
        height: 100%;
        & > * {
            height: 100%;
        }
    }
</style>
