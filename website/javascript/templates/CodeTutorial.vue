<template>
    <Walkthrough title="Beginner Tutorial">
        <template slot="steps">
            <Step title="The Starter Bot" name="starter-bot">
                <p>Now let's write some code!</p>
                <p>
                    This is the starter kit and basic bot code. Your
                    bot is in the "MyBot" file, while the helper
                    library is the "hlt" module. All files are shown
                    in the left pane, the file being edited is in the
                    middle, and the visualizer and game output is on
                    the right. (This interface is available outside
                    the tutorial for simple bot development on-the-go.)
                </p>
                <p>
                    At the moment, this bot does nothing. Let's edit
                    it a bit.
                </p>
            </Step>
            <Step title="Initialization" name="initialization">
                <p>
                    At the beginning of a match, the engine sends you
                    the initial map state and your bot ID. You need to
                    send back your bot's name. Edit the starter bot
                    and name it as you please.
                </p>
            </Step>
            <Step title="Basic Movement" name="basic-movement">
                <p>
                    Now let's move your ship around. TODO: what code?
                </p>
            </Step>
            <Step title="Random Movement" name="shipyards">
                <p>
                    Ships can move north, south, east, or west, as
                    you'll recall. Have the bot move the ship randomly
                    in each direction.
                </p>
            </Step>
            <Step title="Halite Filtration" name="tunneling">
                <p>
                </p>
            </Step>
            <Step title="Depositing Halite" name="dropoffs">
                <p>
                </p>
            </Step>
            <Step title="Better Movement" name="dropoffs">
                <p>
                </p>
            </Step>
            <Step title="Carrier Has Arrived" name="dropoffs">
                <p>
                    One ship does not make a fleet.
                </p>
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
