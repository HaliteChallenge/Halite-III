<template>
<Walkthrough title="Code Tutorial">
  <template slot="steps">
    <Step title="The Starter Bot" name="starter-bot">
      <p>
        This tutorial guides you through creating your first Halite III bot. You’ll write code that tells your bot to build ships, explore the ocean, and collect halite.
      </p>

      <p>
        Let’s take a look at the web editor. The visualizer and game output is on the far right, and your code is in the center. This web editor is available outside this tutorial.
      </p>

      <p>
        The starting code imports some helper library files from the “hlt” file which let your code interact with the game engine. We also import “random” and “logging” to get started - we will talk about those later.
        <p>
          At the moment, this bot doesn’t do very much, so let’s get started.
        </p>
    </Step>
    <Step title="Initialization" name="initialization">
      <p>
        At the beginning of a new game, the engine sends your code the initial map state and your bot’s ID. It waits for you to send back your bot’s name. This initialization step kicks off a game of Halite III.
      </p>

      <p>
        Edit the starter bot and name it whatever you’d like. The line of code to edit is highlighted to help you find it.
      </p>

      <p>
        As you improve your bot, you might want to compute some things that will be useful during the game. Run that code before you send your bot name. Once the engine receives your name, it considers you ready, and the game loop begins.
      </p>
    </Step>
    <Step title="The Game Loop" name="game-loop">
      <p>
        Now that we’re set up, it’s time to run the game. The game changes every turn, and and we get the newest information by running <code>game.update_frame()</code>. We extract some key information and store it in the variables <code>me</code>        and <code>game_map</code> to give us fast access to important parts of the game state.
      </p>

      <p>
        Now we have the most current details of the map, which includes how much halite is in the sea at each location, the location and halite quantity of each ship on the map, and how much halite each player has in storage.
      </p>

      <p>
        Each turn, you process this information and come up with a new set of moves according to your strategy. The last step of the game loop is to send your moves back to the game engine with
        <code>game.end_turn(command_queue)</code>.
      </p>

      <p>
        We repeat this loop every turn until the game is over. A small map has 400 turns in a game, but turns increase with map size up to 500. You can always find out which turn you are on by using the
        <code>game.turn_number</code> variable.
      </p>

      <p>
        Run a game using the button below. You’ll see two players on the map. Your opponent is a clone of your code. Your bot builds a ship in your shipyard, but doesn’t do much else.
      </p>

      <button class="run-game" v-on:click="runGame">
        PLAY NOW
        <img src="/assets/images/icons/pixelate-arrow.png" alt="play"/>
      </button>
    </Step>
    <Step title="Basic Movement" name="basic-movement">
      <p>
        Let’s get your ship moving! The highlighted code will loop through your list of ships, giving you an opportunity to tell each ship what to do this turn. We’ll store those commands into the variable
        <code>command_queue</code>. The code in purple sends the full list of moves for every ship you have to the engine, ending your turn.
      </p>

      <p>
        The <code>ship.move(direction)</code> function accepts the single letter strings <code>“n”, “s”, “e”, “w”</code>, which correspond to the cardinal directions, as its argument. It costs halite to move. You might see some warnings about this in the log. If your ship does not have enough halite to move on a turn, it will stay still and collect halite. You can tell the ship to do this manually with the <code>”o”</code> move command.
      </p>

      <p>
        Try out different movement strategies, like always moving west. Remember that the map edges wrap, so ships that move off the left edge will reappear on the right edge.
      </p>

      <p>
        You can run games as often as you like.
      </p>

      <button class="run-game" v-on:click="runGame">
        PLAY NOW
        <img src="/assets/images/icons/pixelate-arrow.png" alt="play"/>
      </button>
    </Step>
    <Step title="Build Your Fleet" name="more-ships">
      <p>
        One ship does not make a fleet. For 1,000 halite, you can build a new ship at your shipyard. Let’s change the code to create more ships.
      </p>

      <p>

      <p>The test
        <code>me.halite_amount >= SHIP_COST</code> ensures that you have enough halite to build a ship. The test
        <code>game_map[me.shipyard].is_occupied</code> makes sure that the shipyard is empty before you build a ship there - two ships in one square results in maritime disaster!
      </p>

      <p>
        Part of your strategy might be to limit ships to a certain number, or to only build ships in certain phases of the game. You’ll want to think carefully about hoarding halite versus spending it on new ships. The winner is ultimately the one with the most
        stored halite.
      </p>
      <button class="run-game" v-on:click="runGame">
        PLAY NOW
        <img src="/assets/images/icons/pixelate-arrow.png" alt="play"/>
      </button>
    </Step>
    <Step title="Halite Collection" name="dropoffs">
      <p>
        Now that we’ve got more ships, let’s get some information on their most important job: collecting halite. Right now, a ship moves randomly each turn, and collects halite when it is close to empty.
      </p>

      <p>
        Let’s send a message to ourselves at this part of the code so that we can see how much halite there is in the ship’s cargo. Add this code inside the <code>for ships</code> loop but above the movement if/else block.
      </p>

      <code>
logging.info("Ship {} has {} halite.".format(ship.id, ship.halite_amount))</code>

      <p>
        Logging is how your bot communicates with you. We use this logging method because the bot reserves the regular STDOUT (print statements) to communicate with the game engine. You can read logs under any error messages in the panel under the visualizer - scroll all the way down to see your logging statements.
      </p>

      <p>
        Now your bot will tell you how much halite each ship currently holds. Ships can hold a maxiumum of 1,000 halite. When your ship collects halite on its turn, it will collect 25% of halite available in its location, rounded up. You might want to experiment with different conditions for when to collect halite. An efficient collection strategy will build your reserves of halite quickly, and logging is there
        to help you debug your code.
      </p>

      <button class="run-game" v-on:click="runGame">
        PLAY NOW
        <img src="/assets/images/icons/pixelate-arrow.png" alt="play"/>
      </button>
    </Step>
    <Step title="Depositing Halite" name="dropoffs">
      <p>
        You can see now that your ships are filling up with halite. You won't receive credit for the halite until it is brought back to port, so let’s add another condition: if a ship has a decent cargo of halite, send it back to the shipyard.
      </p>

      <p>
        Right now, your bot is stateless. It moves depending on conditions that are only true in this turn; it doesn’t know what it did on previous turns, and it can’t save knowledge to use on future turns.
      </p>

      <p>
        This design won’t work if we want to give ships destinations. We’ll need to save a destination and continue reminding the ship to go there until it arrives. Without this, ships won’t return to the shipyard in a useful, predictable way. So let’s create
        a place to store directions for each ship.
      </p>

      <p>
        Add the following code right after initialization:
      </p>

      <pre>ship_status = {}</pre>

      <p>
        And add the following code inside the ship loop, before the movement block.
      </p>

      <pre>
if ship.id not in ship_status:
    ship_status[ship.id] = "exploring"

if ship_status[ship.id] == "returning":
    if ship.position == me.shipyard.position:
        ship_status[ship.id] = "exploring"
    else:
        move = game_map.naive_navigate(ship, me.shipyard.position)
        command_queue.append(ship.move(move))
        continue
elif ship.halite_amount >= constants.MAX_HALITE / 4:
    ship_status[ship.id] = "returning"</pre>

      <p>
        This code creates two new missions for ships, and provides instructions to ships depending on which mission they are assigned to. We are using some methods that are useful in moving ships around the map: <code>ship.move(direction)</code> and <code>game_map.naive_navigate(ship,
                destination)</code>. We’ll talk in-depth about movement in the Next Steps tutorials.
      </p>

      <button class="run-game" v-on:click="runGame">
        PLAY NOW
        <img src="/assets/images/icons/pixelate-arrow.png" alt="play"/>
      </button>
    </Step>
    <Step title="Build a Dropoff Point" name="carrier">
      <p>
        Now you have exploring ships finding halite, and returning ships bringing the glowing riches back to your shipyard. Let’s make collecting halite more efficient by building dropoff points where your ships can empty their cargo.
      </p>

      <p>
        You can build dropoff points anywhere on the map; all you need is a ship in the desired location, and 4,000 halite available in your stored halite. You’ll convert a ship to a dropoff point, so be aware that this action consumes a ship, any halite cargo on that ship, and sets the dissolved halite in the sea beneath the dropoff to zero.  A dropoff is a permanent feature of the game map, so no one can build a dropoff on top of yours.
      </p>

      <p>
        <code>command_queue.append(ship.make_dropoff())</code> builds a dropoff.
      </p>

      <p>
        Add this functionality to your code, and try to get your bots using these dropoffs when they are on a “returning” mission. You might find these methods from “hlt.py” helpful:
      </p>
      <p>
        <code>me.get_dropoffs()</code> returns a list of all your dropoffs.
      </p>
      <p>
        <code>game_map.calculate_distance(ship.position,
                dropoff.position)</code> will return the Manhattan distance calculated between a ship and a given dropoff.
      </p>

      <p>
        Experiment with different strategies to build dropoffs where and when they are most helpful; a smart construction policy can really accelerate your halite collection.
      </p>

      <button class="run-game" v-on:click="runGame">
        PLAY NOW
        <img src="/assets/images/icons/pixelate-arrow.png" alt="play"/>
      </button>
    </Step>
    <Step title="Start Playing!" name="collision-avoidance">
      <p>
        Your bot works! At this point, your bot explores the map, collects halite, and brings it back to port. It should also build new ships and dropoffs according to your criteria. Let’s submit your bot so that it can start playing against other bots. If you’re
        stuck, we’ve put together a guide to help you <b><a href="/learn-programming-challenge/developing-a-bot#debugging" target="_blank"> debug your bot</a></b>, or check out our <b><a href="https://forums.halite.io/" target="_blank">forums</a></b> to get help from other players.
      </p>
      <p v-if="uploadMessage">
        <strong>{{uploadMessage}}</strong>
      </p>
      <button v-on:click="upload" v-else>
                    Upload
                </button>
      <br/><br/>
      <p>
        You’ll get an email when your bot starts playing. Check out your profile to watch your games!
      </p>

      <p>
        <h2>What’s next?</h2>
      </p>
      <p>
        There’s lots to do next.
      </p>
      <p>
        If you want more guidance in improving your bot, there are more tutorial steps available:
      </p>
      <p>
      <b><a href="/learn-programming-challenge/tutorials/next-tutorial">Next Steps Tutorial</a></b>
</p>
      <p>
        The code you've worked on in this tutorial will be available from our <b><a href="/editor">online editor</a></b>. You can always reset your bot on the editor's settings panel if you would prefer to start from scratch. </p>

      <p>
        You can also <b><a href="/learn-programming-challenge/downloads" target="_blank">download</a></b> the development tools if you would like to work on your bot locally or offline.
      </p>
      <p>
        Halite III also has a great community, and we’re excited to see you there! Join us on the official <b><a href="https://forums.halite.io/" target="_blank">Forums</a></b>.
      </p>

      <button class="run-game" v-on:click="runGame">
        PLAY NOW
        <img src="/assets/images/icons/pixelate-arrow.png" alt="play"/>
      </button>
    </Step>
  </template>

  <template slot="content" slot-scope="walkthrough">
    <CodeTutorialStepper ref="stepper" v-bind:progress="walkthrough.progress" v-bind:step-name="walkthrough.stepName" />
  </template>
</Walkthrough>
</template>

<script>
import Walkthrough from './Walkthrough.vue'
import WalkthroughStep from './WalkthroughStep.vue'
import CodeTutorialStepper from './CodeTutorialStepper.vue'

export default {
  name: 'beginner-tutorial',
  components: {
    'Walkthrough': Walkthrough,
    'Step': WalkthroughStep,
    'CodeTutorialStepper': CodeTutorialStepper
  },
  data: function () {
    return {
      uploadMessage: null
    }
  },
  mounted: function () {

  },
  methods: {
    upload: function () {
      this.uploadMessage = 'Uploading...'
      this.$refs.stepper.upload().then(() => {
        this.uploadMessage = 'Success!'
      }, () => {
        this.uploadMessage = 'Hmm, something went wrong.'
      })
      window.ga('send', 'event', 'tutorial', 'tutorial-flow', 'click-upload-bot')
    },
    runGame: function () {
      this.$refs.stepper.runGame({
        width: 32,
        height: 32
      })
      window.ga('send', 'event', 'tutorial', 'tutorial-flow', 'click-run-game')
    }
  }
}
</script>

<style lang="scss" scoped>
  .steps{
    background: #F3F3F3
  }
</style>

<style lang="scss">
.walkthrough-body .container[role="main"] {
  margin: 0;
  padding: 0;
  width: 100%;
}

.walkthrough {
  height: calc(100vh - 61px);
  /* TODO: Need to keep in sync with layout */
}
</style>
