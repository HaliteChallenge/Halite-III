<template>
    <div class="map-props">
        <div class="map-props-list">
            <table>
              <tr>
                <th colspan="2">Selected Point</th>
              </tr>
                <tr>
                    <td>Location:</td>
                    <td>{{info.location}}</td>
                </tr>
                <tr>
                    <td>Halite:</td>
                    <td>{{info.energy}}</td>
                </tr>
            </table>
        </div>
    </div>
</template>
<script>
 import Vue from 'vue'

export default {
   props: ['selectedPoint', 'players'],
   computed: {
     info: function () {
       const info = {
         location: `${this.selectedPoint.x}, ${this.selectedPoint.y}`,
         energy: this.selectedPoint.energy,
       }

       return info
     }
   }
 }
</script>


<style scoped>
  th {
    padding: 5px 0;
  }
</style>
