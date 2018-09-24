<template>
  <div class="player-cards-list row">
    <div class="col-sm-6" v-for="(player,index) in statistics" :key="index">
      <div class="card-player">
        <h4 class="card-player-name">
          {{replay.players[index].name}}
          <span :class="'circle bg-player-' + (parseInt(index)+1)"></span>
        </h4>
        <div class="card-player-stats-list">
          <!-- Replay stores energy at -end- of turn, so use previous frame's energy to be accurate. -->
          <div class="card-player-stats"><span class="icon-lightning"></span> Current Energy: {{frame > 0 ? replay.full_frames[frame - 1].energy[index] : replay.GAME_CONSTANTS.INITIAL_ENERGY}}</div>
          <div class="card-player-stats"><span class="icon-ship"></span> Current Ships: {{player.ships}}</div>
          <div class="card-player-stats"><span class="icon-planet"></span> Dropoffs Built: {{player.planets}}</div>
          <div class="card-player-stats"><span class="icon-lightning"></span> Collisions: {{playerInfo ? playerInfo[index].totalDamages : ''}}</div>
          <div class="card-player-stats"><span class="icon-ships"></span> Total Ships: {{player.shipsProduced}}</div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
  import Vue from 'vue'

export default{
    name: 'PlayerDetailPane',
    props: ['replay', 'frame', 'stats', 'statistics'],
    data: function () {
      return {}
    },
    mounted: function () {
    },
    computed: {
      playerInfo: function () {
        if (!this.stats) return null

        return this.stats.frames[this.frame].players
      }
    },
    methods: {
      numberSep: function (number) {
        return number.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ',')
      }
    }
  }
</script>
<style>

</style>
