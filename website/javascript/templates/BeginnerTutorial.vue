<template>
    <Walkthrough title="Beginner Tutorial" v-bind:steps="steps">
        <template slot-scope="walkthrough">
            <BeginnerTutorialStepper
                v-bind:progress="walkthrough.progress"
                v-bind:step-name="walkthrough.stepName"
            />
        </template>
    </Walkthrough>
</template>

<script>
    import Walkthrough from "./Walkthrough.vue";
    import BeginnerTutorialStepper from "./BeginnerTutorialStepper.vue";

    export default {
        name: "beginner-tutorial",
        components: {
            "Walkthrough": Walkthrough,
            "BeginnerTutorialStepper": BeginnerTutorialStepper,
        },
        data: function () {
            return {
                steps: [
                    {
                        title: "The Seas of Halite",
                        name: "seas",
                        description: "Description.",
                    },
                    {
                        title: "Your Fleet",
                        name: "fleet",
                        description: "This is your fleet.",
                    },
                ],
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
