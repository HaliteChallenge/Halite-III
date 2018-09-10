<template>
<div class="player-cards-list">
  <div v-for="(player,index) in statistics" :key="index">
    <div class="card-player">
      <div class="card-player-left">
        <h4 :class="'card-player-name player-'+ (parseInt(index)+1) ">
          {{replay.players[index].name}}
        </h4>
        <div class="player-current-halite">
          {{frame > 0 ? replay.full_frames[frame - 1].energy[index] : replay.GAME_CONSTANTS.INITIAL_ENERGY}}
          <!-- <span>
            / {{stats && stats.frames[stats.frames.length -1].players[index].depositedHalite}}
          </span> -->
        </div>
        <img class="stats-cube" :src="`/assets/images/visualizer/cube1.png`" alt="cube">
      </div>
      <div class="chat-box"></div>
      <ul class="player-stats-list">
        <li>
          <label>Ship</label>
          <span>{{player.ships}}</span>
        </li>
        <li>
          <label>Drop-off</label>
          <span>{{player.planets}}</span>
        </li>
        <li>
          <label>PlaceHolder</label>
          <span>{{playerInfo ? playerInfo[index].totalDamages : ''}}</span>
        </li>
      </ul>
    </div>
  </div>
</div>
</template>

<script>
import Vue from 'vue'
export default {
  name: 'PlayerDetail',
  props: ['replay', 'frame', 'stats', 'statistics'],
  data: function () {
    return {}
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
    width: 90px;
  }
  .card-player-name{
    font-size: 14px;
    font-weight: 600;
    letter-spacing: 0.53px;
    line-height: 17px;
    text-overflow: unset;
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
  .player-stats-list{
    list-style: none;
    margin: 0;
    li{
      line-height: 33px;
      padding-top: 10px;
      border-bottom: 1.2px solid rgba(8,27,83,.1);
      width: 153px;
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

