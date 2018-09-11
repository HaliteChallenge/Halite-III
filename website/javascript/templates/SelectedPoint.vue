<template>
    <div class="map-props">
 <!--   <div class="map-props-icon">
            <span class="icon-planet"></span>
        </div>  -->
        <div class="map-props-list">
            <table>
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
