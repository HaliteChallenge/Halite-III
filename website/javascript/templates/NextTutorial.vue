<template>
<Walkthrough title="Next Step Tutorials">
  <template slot="steps">
    <Step title="Better Movement" name="better-movement">
      <p>
        Ships often don’t notice halite-rich waters next to them, and move randomly away from the best spots on the map. Let’s write an improvement that chooses the best cell from the five available on each turn: the cells to the north, south, east, west, and the cell the ship is currently on.
      </p>
      <p>
        You will want to loop through the available options and compare them, choosing the cell with the most halite. Then, instead of sending a random choice to the game engine, you’ll send `command_queue.append(ship, best_nearby_choice)`.
      </p>
      <p>
        You might find a couple API methods helpful:
      <ul>
        <li><code>game_map[position].directional_offset(test_direction)</code> will return a location one square in the test_direction from your ship.</li>
        <li><code>game_map[position].get_surrounding_cardinals()</code> will return a list of the positions of each cardinal direction from the given position.</li>
        <li><code>game_map[position].halite</code> will return the halite at a given map location.</li>
      </ul>
      </p>
      <p>
        Moving across heavy concentrations of halite costs more. This is because the movement cost is 10% of the halite in the cell your ship started the turn in, which is deducted from your ship’s halite cargo. Travelling on lower halite cells might be another efficient movement strategy to consider.

      </p>

    </Step>
    <Step title="Pathfinding" name="pathfinding">
      <p>
        TODO: a placeholder for a discussion of bfs in Halite III.
      </p>
      <p>
        Your code currently uses <code>gamemap.basic_move(source, destination)</code> to move ships without colliding. This method avoids collision by checking if the locations that take a shaip closer to its destination are currently occupied or not. An alternative method for movement is available that uses breadth-first search to ensure a clear path toward a target. Let’s look at the <code>gamemap.get_safe_move(source, destination)</code> and see how it improves basic_move.
      </p>
      <p>
<code>get_safe_move</code> uses breadth first search to look through the entire map, marking cells as explored. Another method, <code>_get_first_move</code>, looks backward from the reached destination for the shortest route seen during the bfs. The first steps on that path are the candidates for the direction the ship should move in this turn. You can inspect these functions in the "hlt" folder.

      </p>
    </Step>

    <Step title="Player Interaction" name="player interaction">
      <p>
      There are three ways to directly interact with other players in a match of Halite III: collisions, capture, and inspiration. The Collision Avoidance tutorial covers collisions, so we’ll talk about capture and inspiration here.
      </p>
      <br>
      <h2>Inspiration</h2>
      <p>
        When a ship is outnumbered by nearby opposing players, it becomes inspired, increasing its halite collection efficiency. This ship is motivated to fill its cargo hold with halite and get to safety!
      </p>

      <img src="/assets/images/map_inspiration.gif" alt="TODO: an image showing an inspired ship.">

      <br>
      <h2>Capture</h2>
      <p>When a ship is outnumbered and flanked by an opposing player’s ships, it is captured and changes owners. This mechanic works like capture in the board game Othello/Reversi.
      </p>

      <img src="/assets/images/game-2.jpg" alt="some gifs/images showing successful capture formations">

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
