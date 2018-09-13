<template>
<div class="player-cards-list">
  <div v-for="(player,index) in statistics" :key="index">
    <div class="card-player">
      <div class="card-player-left">
        <h4 :class="'card-player-name player-'+ (parseInt(index)+1) ">
          {{replay.players[index].name}}
        </h4>
        <div class="player-current-halite" :title="extraPlayerStats(index)">
          {{frame > 0 ? replay.full_frames[frame - 1].energy[index] : replay.GAME_CONSTANTS.INITIAL_ENERGY}}
          <!-- <span>
            / {{stats && stats.frames[stats.frames.length -1].players[index].depositedHalite}}
          </span> -->
        </div>
        <img class="stats-cube" :src="`/assets/images/visualizer/cube1.png`" alt="cube">
      </div>
      <div class="chat-box">
        <PlayerHaliteChart :chartData="chartData[index]" :maxY="maxY" :index="frame"/>
      </div>
      <ul class="player-stats-list">
        <li>
          <label>Ships</label>
          <span>{{stats.frames[frame].players[index].currentShips}}</span>
        </li>
        <li>
          <label>Dropoffs</label>
          <span>{{stats.frames[frame].players[index].currentDropoffs}}</span>
        </li>
        <li>
          <label>Collisions</label>
          <span>{{stats.frames[frame].players[index].currentCollisions}}</span>
        </li>
      </ul>
    </div>
  </div>
</div>
</template>

<script>
import Vue from 'vue'
import * as d3 from 'd3'
import PlayerHaliteChart from './PlayerHaliteChart.vue'
export default {
  name: 'PlayerDetail',
  props: ['replay', 'frame', 'stats', 'statistics', 'chartData'],
  data: function () {
    return {}
  },
  components: {
    PlayerHaliteChart
  },
  mounted: function () {
  },
  computed: {
    playerInfo: function () {
      if (!this.stats) return null

      return this.stats.frames[this.frame].players
    },
    maxY() {
      if (!this.chartData || !this.chartData.length) {
        return 0;
      }
      return d3.max(this.chartData.map(player => d3.max(player, (d) => d.y)), (y) => y);
    },
  },
  methods: {
    numberSep: function (number) {
      return number.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ',')
    },
    extraPlayerStats(index) {
      const playerStats = this.replay.game_statistics.player_statistics[index];
      return `Efficiency ${(playerStats.mining_efficiency * 100).toFixed(1)}%; total halite collected ${playerStats.total_mined}, inspiration bonus halite ${playerStats.total_bonus}, captured ships collected ${playerStats.total_mined_from_captured} halite`;
    },
  }
}
</script>
<style lang="scss" scoped>
.player-cards-list{
  padding: 0;
  .card-player{
    margin: 0;
    padding: 10px 20px;
    padding-right: 0;
    color: #0C0C0C;
    display: flex;
    justify-content: space-between;
    border-bottom: 1.2px solid rgba(8,27,83,.1);
  }
  .card-player-left{
    min-width: 125px;
  }
  .card-player-name{
    font-size: 14px;
    font-weight: 600;
    letter-spacing: 0.53px;
    line-height: 17px;
    &.player-1{
      color: #1BB15A;
    }
    &.player-2{
      color: #966AFB;
    }
    &.player-3{
      color: #F54356;
    }
    &.player-4{
      color: #FABB2C;
    }
  }
  .player-current-halite{
    font-size: 28px;
    line-height: 34px;
  }
  .stats-cube{
    width: 60px;
    margin-top: 10px;
  }
  .chat-box{
    width: 100%;
  }
  .player-stats-list{
    list-style: none;
    margin: 0;
    padding-left: 10px;
    li{
      line-height: 33px;
      padding-top: 10px;
      border-bottom: 1.2px solid rgba(8,27,83,.1);
      width: 170px;
      font-size: 18px;
      padding-right: 20px;
      &:last-child{
        border: none;
      }
      span{
        float: right;
        color: #003587;
      }
    }
  }
}
</style>

<!--
     Local Variables:
     web-mode-script-padding: 0
     End:
     End: -->
