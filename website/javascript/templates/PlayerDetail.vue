<template>
<div class="player-cards-list">
  <div v-for="(player,index) in statistics" :key="index">
    <div class="card-player">
      <div class="card-player-left">
        <h4 :class="'card-player-name player-'+ (parseInt(index)+1) ">
          {{replay.players[index].name}}
        </h4>
        <div class="player-current-halite">
          {{currentHalite[index]}}
          <span class="current-percent" :class="(totalHalite > 0 && currentHalite[index]/totalHalite > 0.5)? 'green':'red'">
            {{totalHalite > 0 ? ((currentHalite[index]/totalHalite) * 100).toFixed(2)  + '%' : ''}}
          </span>
        </div>
        <img class="stats-cube" :src="`/assets/images/visualizer/cube1.png`" alt="cube">
      </div>
      <div class="chat-box">
        <PlayerHaliteChart :chartData="chartData[index]" :index="frame"/>
      </div>
      <ul class="player-stats-list">
        <li>
          <label>Ships</label>
          <span>{{player.ships}}</span>
        </li>
        <li>
          <label>Dropoffs</label>
          <span>{{player.planets}}</span>
        </li>
        <li>
          <label>Collisions</label>
          <span>{{playerInfo ? playerInfo[index].totalDamages : ''}}</span>
        </li>
      </ul>
    </div>
  </div>
</div>
</template>

<script>
import Vue from 'vue'
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
  mounted: function () {},
  computed: {
    playerInfo: function () {
      if (!this.stats) return null

      return this.stats.frames[this.frame].players
    }
  },
  methods: {
    numberSep: function (number) {
      return number.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ',')
    },

  },
  computed: {
    totalHalite() {
      let result = 0
      if(this.frame > 0) {
        let energy = this.replay.full_frames[this.frame - 1].energy
        for(let item in energy) {
          result += energy[item]
        }
      }
      return result
    },
    currentHalite() {
      let result = {
      }
      for(let index in this.statistics) {
        result[index] = this.frame > 0? this.replay.full_frames[this.frame - 1].energy[index]: this.replay.GAME_CONSTANTS.INITIAL_ENERGY
      }
      return result
    },
    playerInfo: function () {
      if (!this.stats) return null

      return this.stats.frames[this.frame].players
    }
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
    min-width: 100px;
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
    position: relative;
    .current-percent{
      font-size: 16px;
      display: block;
      position: absolute;
      right: -10px;
      margin-top: -10px;
      &.red {
        color: red;
      }
      &.green{
        color: green;
      }
    }
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

