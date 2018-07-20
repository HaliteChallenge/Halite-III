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
                    and name it as you please&mdash;look at the
                    highlighted line.
                </p>
            </Step>
            <Step title="Basic Movement" name="basic-movement">
                <p>
                    Now let's move your ship around. The code in <span
                    class="tutorial-highlight">red</span> builds up a
                    set of moves by iterating through each ship you
                    have. The code in <span
                    class="tutorial-highlight-alt">yellow</span> then
                    sends those moves to the engine, ending your turn.
                </p>
                <p>
                    Right now, the bot moves ships around
                    randomly. Try changing it to do something like
                    moving the ship always west.
                </p>
            </Step>
            <Step title="Halite Filtration" name="tunneling">
                <p>
                    Moving around burns a lot of halite! Let's have
                    the ship actually collect halite from the
                    sea. Ships will automatically collect halite when
                    they are not told to move.
                </p>
                <p>
                    Python: convert the dictionary comprehension to an
                    actual loop. Then, add some criteria to decide if
                    the ship should move. For instance, you might stop
                    the ship if its cargo/fuel is below a certain
                    level, or if the tile you're on is especially
                    rich. Some APIs that might help:
                </p>
                <ul>
                    <li><code>game_map.foo</code></li>
                    <li><code>entity.bar</code></li>
                </ul>
            </Step>
            <Step title="Depositing Halite" name="dropoffs">
                <p>
                    Your ship now carries and collects halite, but
                    it's not of much use to you until it's brought
                    back to port. Let's add another condition: if the
                    ship has too much halite, start moving it towards
                    the shipyard.
                </p>
                <p>
                    Python: add the following check:
                </p>
                <pre>
code block
this is actually kind of annoying,
because without state, the ship
will go above the threshold, move,
fall below the threshold, then
start doing something else again
                </pre>
            </Step>
            <Step title="Better Movement" name="dropoffs">
                <p>
                    By now, you've probably noticed that your ships
                    aren't moving very efficiently. Recall that moving
                    off of a tile costs you halite proportional to how
                    rich it is.
                </p>
            </Step>
            <Step title="Carrier Has Arrived" name="carrier">
                <p>
                    One ship does not make a fleet. Besides issuing
                    movement commands to ships, you can also
                    commission new ships&mdash;for an appropriate fee,
                    of course. Building a new ship costs 255 halite,
                    plus the initial amount of fuel for the ship.
                    For now, let's build ships whenever possible.
                </p>
                <p>
                    Python: add the following code:
                </p>
                <p>
                    Of course, in a real match, you'll want to think
                    about whether you should be hoarding halite or
                    spending it, because ultimately, the winner is the
                    one with the most halite in the bank.
                </p>
            </Step>
            <Step title="Start Playing!" name="submit">
                <p>
                    You're all done! Let's submit your bot:
                </p>
                <button>
                    Upload
                </button>
                <p>
                    You'll get an email once your bot is processed and
                    starts playing. Look at <a href="#">your
                    profile</a> to watch your games!
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
