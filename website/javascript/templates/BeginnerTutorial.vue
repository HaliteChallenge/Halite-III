<template>
    <Walkthrough title="Beginner Tutorial">
        <template slot="steps">
            <Step title="The Seas of Halite" name="seas">
                <p>
                    Halite is played on a grid, which wraps around at
                    the edges. (Thus, the map is the surface of a torus.)
                </p>
                <h3>Tasks</h3>
                <ol>
                    <li>Drag the map around and observe the wrapping.</li>
                    <li>Zoom in to examine the map more closely.</li>
                    <li>Reset the map to its initial state.</li>
                </ol>
            </Step>
            <Step title="Saltwater" name="salt">
                <p>
                    In these seas are vast quantities of the material
                    known as <em>halite</em>.
                </p>
                <h3>Tasks</h3>
                <ol>
                    <li>Click on a cell and observe how much halite is present.</li>
                </ol>
            </Step>
            <Step title="Admiral" name="admiral">
                <p>
                    Your vessels can filter this halite from the
                    water. Your goal is to go forth and bring back as
                    much halite as possible.
                </p>
            </Step>
            <Step title="A Vessel" name="vessel">
                <p>
                    Each vessel has a certain amount of halite, which
                    is both its cargo and its fuel. They may move in
                    the cardinal directions&mdash;north, south, east,
                    and west&mdash;at a price proportional to how much
                    halite is in the cell they are moving out of. (Dissolved
                    halite changes the viscosity of the seas and makes
                    propulsion difficult.)
                </p>
                <h3>Tasks</h3>
                <ol>
                    <li>Click on a ship and observe how much halite it has.</li>
                </ol>
            </Step>
        </template>
        <template slot="content" slot-scope="walkthrough">
            <BeginnerTutorialStepper
                v-bind:progress="walkthrough.progress"
                v-bind:step-name="walkthrough.stepName"
            />
        </template>
    </Walkthrough>
</template>

<script>
    import Walkthrough from "./Walkthrough.vue";
    import WalkthroughStep from "./WalkthroughStep.vue";
    import BeginnerTutorialStepper from "./BeginnerTutorialStepper.vue";

    export default {
        name: "beginner-tutorial",
        components: {
            "Walkthrough": Walkthrough,
            "Step": WalkthroughStep,
            "BeginnerTutorialStepper": BeginnerTutorialStepper,
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
        width: 100%;
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
