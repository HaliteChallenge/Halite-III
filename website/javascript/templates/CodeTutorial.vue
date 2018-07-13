<template>
    <Walkthrough title="Beginner Tutorial">
        <template slot="steps">
            <Step title="Initialization" name="initialization">
                <p>This is how we talk about initialization.</p>
                <p>This is how we talk about initialization.</p>
            </Step>
            <Step title="Movement" name="movement">
                <p>This is how we talk about initialization.</p>
                <p>This is how we talk about initialization.</p>
            </Step>
        </template>

        <template slot="content" slot-scope="walkthrough">
            <CodeTutorialStepper
                v-bind:progress="walkthrough.progress"
                v-bind:step-name="walkthrough.stepName"
            />
        </template>
    </Walkthrough>
</template>

<script>
    import Walkthrough from "./Walkthrough.vue";
    import WalkthroughStep from "./WalkthroughStep.vue";
    import CodeTutorialStepper from "./CodeTutorialStepper.vue";

    export default {
        name: "beginner-tutorial",
        components: {
            "Walkthrough": Walkthrough,
            "Step": WalkthroughStep,
            "CodeTutorialStepper": CodeTutorialStepper,
        },
        data: function () {
            return {};
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
