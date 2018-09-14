<template>
<Walkthrough title="Next Step Tutorials">
  <template slot="steps">
    <Step title="Better Movement" name="better-movement">
      <p>
        Ships often don’t notice halite-rich waters next to them, and move randomly away from the best spots on the map. Let’s write an improvement that chooses the best cell from the five available on each turn: the cells to the north, south, east, west, and the cell the ship is currently on.
      </p>
      <p>
        You will want to loop through the available options and compare them, choosing the cell with the most halite. Then, instead of sending a random choice to the game engine, you’ll send `command_queue.append(ship.move(direction_towards_best_nearby_choice))`.
      </p>
      <p>
        You might find a couple API methods helpful:
      <ul>
        <li><code>game_map[position].directional_offset(test_direction)</code> will return a location one square in the test_direction from your ship.</li>
        <li><code>game_map[position].get_surrounding_cardinals()</code> will return a list of the positions of each cardinal direction from the given position.</li>
        <li><code>game_map[position].halite_amount</code> will return the halite at a given map location.</li>
      </ul>
      </p>
      <p>
        Moving across heavy concentrations of halite costs more. This is because the movement cost is 10% of the halite in the cell your ship started the turn in, which is deducted from your ship’s halite cargo. Travelling on lower halite cells might be another efficient movement strategy to consider.

      </p>

      <button class="run-game" v-on:click="runGame">
        PLAY NOW
        <img src="/assets/images/icons/pixelate-arrow.png" alt="play"/>
      </button>
    </Step>

    <Step title="Inspiration" name="player interaction">
      <p>
      There are two ways to directly interact with other players in a match of Halite III: collisions, and inspiration. We’ll talk about inspiration here.
      </p>
      <br>
      <h2>Inspiration</h2>
      <p>
        When a ship is outnumbered by two or more nearby opposing players within a four-cell radius, it becomes inspired. An inspired ship collects halite from the sea at the normal rate, and receives an additional 200% bonus. This ship is motivated to fill its cargo hold with halite and get to safety!
      </p>

      <img src="/assets/images/map_inspiration.gif" alt="TODO: an image showing an inspired ship.">

      <br/>

      <button class="run-game" v-on:click="runGame">
        PLAY NOW
        <img src="/assets/images/icons/pixelate-arrow.png" alt="play"/>
      </button>
    </Step>


  </template>

  <template slot="content" slot-scope="walkthrough">
    <NextTutorialStepper ref="stepper" v-bind:progress="walkthrough.progress" v-bind:step-name="walkthrough.stepName" />
  </template>
</Walkthrough>
</template>

<script>
import Walkthrough from './Walkthrough.vue'
import WalkthroughStep from './WalkthroughStep.vue'
import NextTutorialStepper from './NextTutorialStepper.vue'

export default {
  name: 'beginner-tutorial',
  components: {
    'Walkthrough': Walkthrough,
    'Step': WalkthroughStep,
    'NextTutorialStepper': NextTutorialStepper
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
    },
    runGame: function () {
      this.$refs.stepper.runGame({
        width: 32,
        height: 32
      })
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

// .walkthrough-nav {
//   h1,
//   h2,
//   p {
//     padding-left: 0.5rem !important;
//   }
// }
</style>
