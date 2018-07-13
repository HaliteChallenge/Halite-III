<template>
    <Walkthrough title="Beginner Tutorial" v-bind:steps="steps">
        <template slot-scope="walkthrough">
            <CodeTutorialStepper
                v-bind:progress="walkthrough.progress"
                v-bind:step-name="walkthrough.stepName"
            />
        </template>
    </Walkthrough>
</template>

<script>
    import Walkthrough from "./Walkthrough.vue";
    import CodeTutorialStepper from "./CodeTutorialStepper.vue";

    export default {
        name: "beginner-tutorial",
        components: {
            "Walkthrough": Walkthrough,
            "CodeTutorialStepper": CodeTutorialStepper,
        },
        data: function () {
            return {
                steps: [
                    {
                        title: "Initialization",
                        name: "initialization",
                        description: "Description.",
                    },
                    {
                        title: "Issuing Commands",
                        name: "commands",
                        description: "Description.",
                    },
                    {
                        title: "Issuing Commands",
                        name: "commands",
                        description: "Description.",
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
            padding-left: 0.5rem !important;
        }
    }
</style>
