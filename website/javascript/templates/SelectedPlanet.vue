<template>
<div class="map-props">
  <div class="map-props-icon">
    <span class="icon-planet"></span>
  </div>
  <div class="map-props-list">
    <table>
      <tr>
        <td>Location:</td>
        <td>{{info.location}}</td>
      </tr>
      <tr>
        <td>Owner:</td>
        <td :class="`color-${selectedPlanet.owner + 1}`">{{info.owner}}</td>
      </tr>
      <tr>
        <td>Type:</td>
        <td>{{info.type}}</td>
      </tr>
      <tr>
        <td>Dissolved Halite:</td>
        <td>0</td>
      </tr>
    </table>
  </div>
</div>
</template>
<script>
  import Vue from 'vue'
import {isUndefined, isNull} from 'lodash'

export default {
    props: ['selectedPlanet', 'players'],
    computed: {
      info: function () {
        const base = this.selectedPlanet

        return {
          location: `${base.x}, ${base.y}`,
          owner: this.players[base.owner].name,
          type: base.type
        }
      }
    }
  }
</script>
