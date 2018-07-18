<template>
    <section>
        <Visualizer
            v-if="replay"
            v-bind:replay="replay"
            v-bind:game="game"
        />
        <p>{{progress}}</p>
        <p>{{stepName}}</p>
        <template v-if="stepName === 'seas'">
            <button v-on:click="completeSubstep('seas-pan')">Pan map</button>
            <button v-on:click="completeSubstep('seas-zoom')">Zoom map</button>
            <button v-on:click="completeSubstep('seas-reset')">Reset map</button>
        </template>
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
                  .then((req) => req.json())
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
