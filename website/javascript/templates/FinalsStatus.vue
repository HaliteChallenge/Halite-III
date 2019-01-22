<template>
  <div class="finals-status">
    <div v-if="submissions_open" style="text-align: center; padding: 1em;">
        <h2>FINALS SCHEDULE</h2>
        <br>
        <p>Halite finals run for just under one week, from January 22, 11:59:59PM ("midnight") EST to January 29, 11:59:59AM ("noon") EST. The games played during the finals period will determine the final rankings, and thus the winner, of the Halite III competition.
        </p>
        <p>Bot submission is closed during the finals period. Shortly after finals begin, all matchmaking will be paused and all bot ranks reset.
        </p>
        <p>
        Once bot ranks have been reset, finals matchmaking for all bots in the competition will begin. Over the course of the finals period, the lowest ranking bots will be systematically removed, according to the below games schedule.
        </p>
        <p>The final {{cutoff_schedule[0].end_rank}} remaining bots will play until the close of the finals period, to determine high-confidence final rankings.
        </p>
        <br>
    </div>
    <div v-else style="text-align: center; padding: 1em;">
        <h2>FINALS STATUS</h2>
        <template v-if="!finals_pairing">
          <p> We are resetting all bot rankings to zero, during which time no matches will be made.
          </p>
          <p>Once bot ranks have been reset, finals matchmaking for all bots in the competition will begin. Over the course of the finals period, the lowest ranking bots will be systematically removed, according to the below games schedule.
        </p>
        <p>The final {{cutoff_schedule[0].end_rank}} remaining bots will play until the close of the finals period, to determine high-confidence final rankings.
        </p>
          </p>
        </template>
        <template v-else>
          <h2>Finals run through January 29, noon EST</h2>
        </p>
          <p>Currently, bots are playing finals games. Over the course of the finals period, the lowest ranking bots will be systematically removed from play, according to the below games schedule.<br>
          The final {{cutoff_schedule[0].end_rank}} remaining bots will play until the close of the finals period, to determine high-confidence final rankings.
          </p>
        </template>
    </div>
    <!-- When finals are complete replace the above divs with this one
    <div style="text-align: center; padding: 1em;">
      <p>Finals have ended! Check out the <a href="/programming-competition-leaderboard">leaderboard</a> to see who won.
      </p>
    </div>
    -->
    <table style="width:100%; table-layout:fixed; padding:1em;">
        <tbody>
            <tr>
                <td style="width:50%; text-align:center;">
                    <h2>CURRENT COMPETITION STATUS</h2>
                    <p><strong>Submissions:</strong>
                      <template v-if="submissions_open">
                        OPEN
                      </template>
                      <template v-else>
                        CLOSED
                      </template>
                    </p>
                    <p><strong>Matchmaking:</strong>
                      <template v-if="!finals_pairing">
                        NOT YET STARTED
                      </template>
                      <template v-else>
                        IN PROGRESS, {{finals_games}} PLAYED
                      </template>
                    </p>
                    <p><strong>Ranks Currently Playing:</strong>
                      1-{{current_cutoff}}
                    </p>
                </td>
                <td style="width:50%; text-align:center;">
                    <h2>GAME PROGRESS</h2>
                    <p>The most recent game played is <a :href="'/play?game_id=' + current_game">{{current_game}}.</a></p>
                    <p v-if="next_cutoff">Ranks {{next_cutoff+1}}-{{current_cutoff}} will be eliminated
                      <template v-if="finals_pairing">in</template>
                      <template v-else>after</template>
                      {{games_to_next}} games.
                    </p>
                    <p v-else>Ranks 1-{{current_cutoff}} will continue playing to the end of the finals.</p>
                    <p>Finals end January 29th, 11:59:59AM EST.</p>
                </td>
            </tr>
        </tbody>
    </table>

    <div v-if="cutoff_schedule">
      <br>
      <br>
      <h2 style="text-align:center;">CUTOFF SCHEDULE</h2>
      <table class="table" style="width:60%;margin-left:auto; margin-right:auto;">
        <thead>
          <tr>
            <th>Rank Range</th>
            <th>Playing Currently?</th>
            <th>Number of Games Before Cutoff</th>
            <th v-if="last_open_game">Last Eligible Game ID</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for='cutoff in cutoff_schedule' :key="cutoff" v-bind:class="{'hl': finals_pairing && cutoff.end_rank == current_cutoff}">
            <td>{{cutoff.start_rank}}-{{cutoff.end_rank}}</td>
            <td>
              <template v-if="cutoff.end_rank <= current_cutoff">Yes</template>
              <template v-else>No</template>
            </td>
            <td>{{cutoff.end_game || "Plays to end"}}</td>
            <td v-if="last_open_game">
              <template v-if="cutoff.end_game">
                {{last_open_game + cutoff.end_game}}
              </template>
            </td>
          </tr>
        </tbody>
      </table>
      <p v-if="finals_pairing">* Note that the highlighted group is the next group to be removed from the competition</p>
    </div>
  </div>
</template>

<script>
import Vue from 'vue'
import * as api from '../api'

export default {
  name: 'FinalsStatus',
  props: ['baseUrl'],
  data: function () {
    return {
      submissions_open: null,
      finals_pairing: null,
      current_game: null,
      last_open_game: null,
      cutoff_schedule: null,
      finals_games: null,
      current_cutoff: null,
      games_to_next: null,
    }
  },
  mounted: function () {
    this.fetch()
  },
  methods: {
    fetch: function () {
      const url = `${api.API_SERVER_URL}/finals`
      return new Promise((resolve, reject) => {
        $.get(url).then((data) => {
          console.log(data)
          this.submissions_open = data.submissions_open
          this.finals_pairing = data.finals_pairing
          this.current_game = data.current_game
          this.last_open_game = data.last_open_game
          if (this.last_open_game) {
            this.finals_games = this.current_game - this.last_open_game
          } else {
            this.finals_games = 0
          }
          this.cutoff_schedule = Array()
          for (let i=0; i < data.cutoff_schedule.length; i++) {
            let cutoff = {}
            if (i < data.cutoff_schedule.length - 1) {
              cutoff.start_rank = data.cutoff_schedule[i+1][1] + 1
              cutoff.end_game = data.cutoff_schedule[i+1][0]
            } else {
              cutoff.start_rank = 1;
              cutoff.end_game = null;
            }
            cutoff.start_game = data.cutoff_schedule[i][0]
            cutoff.end_rank = data.cutoff_schedule[i][1]
            this.cutoff_schedule.push(cutoff)
          }
          if (this.finals_games > 0) {
            for (const cutoff of this.cutoff_schedule) {
              if (cutoff.start_game > this.finals_games) {
                this.next_cutoff = cutoff.end_rank
                this.next_start = cutoff.start_game
                this.games_to_next = cutoff.start_game - this.finals_games
                break
              }
              this.current_cutoff = cutoff.end_rank
            }
          } else {
            this.current_cutoff = this.cutoff_schedule[0].end_rank
            this.next_cutoff = this.cutoff_schedule[1].end_rank
            this.games_to_next = this.cutoff_schedule[1].start_game
          }
          this.cutoff_schedule.reverse()

          resolve(data)
        })
      })
    }
  }
}
</script>

<style lang="scss" scoped>
  .finals-status {
    padding: 40px 15px 15px 15px;
    background-color: #fff;
    border-radius: 8px;
    margin-bottom: 30px;
    background-image: url("/assets/images/doc-bg.png");
    background-position: top center;
    background-size: contain;
    background-repeat: no-repeat;
    overflow: hidden;
    color: #52678D;
    .sub-title{
      font-size: 20px;
      font-weight: 600;
      color: #414141;
    }
    h2{
      font-size: 24px;
      font-weight: 600;
      color: #033C89;
    }
    h3{
      font-size: 20px;
      font-weight: 600;
      color: #292929;
      text-transform: none;
      line-height: 1.3em;
    }
  }

.table {
  > thead{
    > tr{
      background: #f1f3f7;
      > th, >td{
        padding: 12px 7px;
        font-size: 12px;
        color: #343434;
        text-transform: uppercase;
        font-weight: 600;
      }
    }
  }
  > tbody{
    > tr{
      > td{
        border-bottom: 1px solid #DBDDDF;
        padding: 12px 7px;
        font-size: 14px;
        a{
          font-weight: 600;
          color: #033C89;
        }
      }
    }
  }
}


</style>