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

<style lang="scss">
    .container[role="main"] {
        margin: 0;
        padding: 0;
    }

    .walkthrough {
        height: calc(100vh - 61px); /* TODO: Need to keep in sync with layout */
    }

    .walkthrough-nav {
        h1, h2, p {
            padding-left: 0.25em;
        }
    }
</style>
