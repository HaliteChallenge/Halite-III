<template>
    <Walkthrough title="Beginner Tutorial">
        <template slot="steps">
            <Step title="The Seas of Halite" name="seas">
                <p>
                    This is the game visualizer. In Halite, the game
                    engine communicates with the bots and runs the
                    game; at the end, a replay of what happened is
                    generated. You then watch the replay with the
                    visualizer.
                </p>
                <p>
                    Halite is played on a grid, which wraps around at
                    the edges. (Thus, the map is the surface of a torus.)
                </p>
                <walkthrough-substeps>
                    <walkthrough-substep name="seas-pan">
                        Drag the map around and observe the wrapping.
                    </walkthrough-substep>
                    <walkthrough-substep name="seas-zoom">
                        Zoom in to examine the map more closely.
                    </walkthrough-substep>
                    <walkthrough-substep name="seas-reset">
                        Reset the map to its initial state.
                    </walkthrough-substep>
                </walkthrough-substeps>
            </Step>
            <Step title="Time" name="time">
                <p>
                    The game progresses in turns: each turn, every
                    player is sent the current game state, they issue
                    commands, which are then processed. This repeats
                    for 500 turns.
                </p>
            </Step>
            <Step title="Saltwater" name="salt">
                <p>
                    In these seas are vast quantities of the material
                    known as <em>halite</em>. The visualizer shows
                    areas with more halite dissolved in the water with
                    red colors, and areas with less halite with blue
                    colors.
                </p>
                <walkthrough-substeps>
                    <walkthrough-substep name="salt-halite">
                        Click on a cell and observe how much halite is present.
                    </walkthrough-substep>
                </walkthrough-substeps>
            </Step>
            <Step title="Admiral" name="admiral">
                <p>
                    You command a fleet of ships, which are capable of
                    filtering halite from the water. Your goal is to
                    go forth and bring back as much halite as
                    possible.
                </p>
            </Step>
            <Step title="Shipyard" name="shipyard">
                <p>
                    Every player has a base of operations, called
                    their shipyard. This stores the halite you
                    collect, and turns halite into new ships for you
                    to use.
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
                <walkthrough-substeps>
                    <walkthrough-substep name="vessel-halite">
                        Click on a ship and observe how much halite it has.
                    </walkthrough-substep>
                </walkthrough-substeps>
            </Step>
            <Step title="Collecting Halite" name="collection">
                <p>
                    When ships are not moving, they can collect halite
                    from the sea by filtering it out of the
                    water. Ships collect 10% of the halite present in
                    the waters around them every turn.
                </p>
                <walkthrough-substeps>
                    <walkthrough-substep name="collection-play">
                        Watch the replay and observe the map as ships
                        collect the halite near their shipyard.
                    </walkthrough-substep>
                </walkthrough-substeps>
            </Step>
            <Step title="Depositing Halite" name="depositing">
                <p>
                    This halite is not safe until it has been
                    deposited in the shipyard. Ships may dump halite
                    overboard; if they do so while on top of their
                    shipyard, that halite will get collected by the
                    player. Otherwise, it returns to the sea.
                </p>
                <p>
                    Replay the game, observing a ship's cargo as it
                    goes around the map.
                </p>
            </Step>
            <Step title="Drop-Off Areas" name="dropoffs">
                <p>
                    Bringing halite from the far corners of the globe
                    can be perilous and slow. Instead, ships may build
                    drop-off points anywhere on the map, for the cost
                    of quite a bit of halite. Then, instead of
                    returning to the shipyard, ships can simply
                    deposit halite at the nearest drop-off.
                </p>
            </Step>
            <Step title="Advanced" name="advanced">
                <p>
                    Drop-off points are not exclusive to the player
                    who built them: other players may use them to
                    deposit halite into their own accounts. Like an
                    ATM, however, they charge hefty fees; a player who
                    deposits halite at another's drop-off will find
                    25% of the cargo redirected into the original
                    player's accounts.
                </p>
                <p>
                    Ships may not pass through each other. Two ships
                    that try to enter the same cell at the same time
                    will both be destroyed; their cargo will be
                    deposited where they collided.
                </p>
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
    .walkthrough-body {
        .container[role="main"] {
            margin: 0;
            padding: 0;
            width: 100%;
        }
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
