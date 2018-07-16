<template>
    <section>
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
    export default {
        name: "beginner-tutorial-stepper",
        props: ["progress", "step-name"],
        inject: ["completeSubstep"],
        data: function () {
            return {
            };
        },
        mounted: function () {

        },
        methods: {

        },
    };
</script>

<style lang="scss" scoped>

</style>
