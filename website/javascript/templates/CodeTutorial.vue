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
                    At the moment, this bot doesn't do much
                    interesting. Let's edit it a bit.
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
                <p>
                    For advanced bots, you might take advantage of
                    this time to process the game map and pre-compute
                    things your bot needs. Just do it before you send
                    back your bot name&mdash;once you do that, the
                    engine considers you ready.
                </p>
                <!-- TODO: need to show name in replay -->
            </Step>
            <Step title="The Game Loop" name="game-loop">
                <p>
                    After initialization, your bot enters an infinite
                    loop of retrieving the current game state,
                    computing moves, and sending them back to the
                    engine. All the magic happens here&mdash;each
                    turn, you'll have to come up with a new set of
                    moves according to your secret strategy, and
                    you'll get the results next turn.
                </p>
                <p>
                    Games last for 500 turns, so it's not really an
                    infinite loop.
                </p>
            </Step>
            <Step title="Basic Movement" name="basic-movement">
                <p>
                    Now let's move your ship around. The <span
                    class="tutorial-highlight">highlighted</span> code
                    builds up a set of moves by iterating through each
                    ship you have. The code in <span
                    class="tutorial-highlight-alt">purple</span> then
                    sends those moves to the engine, ending your turn.
                </p>
                <p>
                    Right now, the bot moves ships around
                    randomly. Try different movement strategies, like
                    always moving the ship west.
                </p>
                <!-- TODO: tell them how to run games -->
            </Step>
            <Step title="Collision Avoidance" name="collision-avoidance">
                <p>
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
                    Python: add some criteria to decide if the ship
                    should move. For instance, you might stop the ship
                    if its cargo/fuel is below a certain level, or if
                    the tile you're on is especially rich. Some APIs
                    that might help:
                </p>
                <ul>
                    <li><code>ship.location</code>&mdash;a two-element tuple of (x, y) coordinates.</li>
                    <li><code>ship.halite</code>&mdash;an <tt>int</tt> indicating how much halite the ship has.</li>
                    <li><code>game_map[&lt;y_coordinate&gt;][&lt;x_coordinate&gt;]</code>&mdash;how much halite is on the given coordinates.</li>
                    <li><code>hlt.MAX_HALITE</code>&mdash;a constant representing the maximum amount of halite a ship can carry.</li>
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
                    Right now, our bot is <em>stateless</em>: it makes
                    its moves purely based on the most recent game
                    information, without tracking any information
                    between turns. However, this isn't going to work
                    for us much longer. In particular, if we add a
                    check that moves ships with more than a certain
                    amount of stored halite towards the shipyard,
                    what'll happen is it'll move a bit, and its halite
                    will drop below the threshold. Then it'll do
                    something else again, until its halite goes back
                    up, but by then, it might have wandered off.
                </p>
                <p>
                    So we need to add a do-or-die check: once we
                    decide to move a ship back to port, we need to
                    commit to it.
                </p>
                <p>
                    Python: add the following code right after
                    initialization:
                </p>
                <pre>
ship_status = {}</pre>
                <p>
                    Add the following code inside the ship loop:
                </p>
                <pre>
if ship.location == players[myID].shipyard:
    commands.dump(ship.id, ship.halite)
    ship_status[ship.id] = "exploring"
elif ship_status.get(ship.id) === "returning":
    # Figure out how to make the ship move towards port.
    pass
elif ship.halite > hlt.MAX_HALITE > 2:
    ship_status[ship_id] = "returning"</pre>
            </Step>
            <Step title="Better Movement" name="dropoffs">
                <p>
                    By now, you've probably noticed that your ships
                    aren't moving very efficiently. For instance,
                    moving off of a tile costs you halite proportional
                    to how rich it is.
                </p>
                <p>
                    Let's make a simple improvement: instead of moving
                    ships randomly, let's move them onto the
                    neighboring cell with the most halite. That way,
                    we can start mining faster, and don't burn fuel
                    moving onto low-halite cells. You might find this
                    helpful:
                </p>
                <ul>
                    <li><code>game_map.location_with_offset(location, direction)</code>&mdash;given a location and a direction (<tt>"n"</tt>, <tt>"s"</tt>, etc.), returns the coordinates of the resulting cell.</li>
                </ul>
            </Step>
            <Step title="Carrier Has Arrived" name="carrier">
                <p>
                    One ship does not make a fleet. Besides issuing
                    movement commands to ships, you can also
                    commission new ships&mdash;for an appropriate fee,
                    of course. Building a new ship costs 500 halite,
                    plus the initial amount of fuel for the ship.
                    Right now, the bot already builds ships whenever
                    possible.
                </p>
                <p>
                    In a real match, you'll want to think about
                    whether you should be hoarding halite or spending
                    it, because ultimately, the winner is the one with
                    the most halite in the bank.
                </p>
                <p>
                    So, let's change the bot to only make ships in the
                    first 200 turns of the game.
                </p>
            </Step>
            <Step title="Start Playing!" name="submit">
                <p>
                    You're all done! At this point, your bot explores
                    the map, mines halite, brings it back to port, and
                    builds new ships when necessary. Let's submit your
                    bot so it can start playing:
                </p>
                <p v-if="uploadMessage">
                    <strong>{{uploadMessage}}</strong>
                </p>
                <button v-on:click="upload" v-else>
                    Upload
                </button>
                <p>
                    You'll get an email once your bot is processed and
                    starts playing. Look at <a href="/user?me">your
                    profile</a> to watch your games!
                </p>
            </Step>
        </template>

        <template slot="content" slot-scope="walkthrough">
            <CodeTutorialStepper
                ref="stepper"
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
            return {
                uploadMessage: null,
            };
        },
        mounted: function () {

        },
        methods: {
            upload: function() {
                this.uploadMessage = "Uploading...";
                this.$refs.stepper.upload().then(() => {
                    this.uploadMessage = "Success!";
                }, () => {
                    this.uploadMessage = "Hmm, something went wrong.";
                });
            },
        },
    };
</script>

<style lang="scss" scoped>

</style>

<style lang="scss">
    .walkthrough-body .container[role="main"] {
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
