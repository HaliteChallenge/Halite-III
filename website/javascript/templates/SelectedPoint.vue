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
                    <td>Cell Production:</td>
                    <td>{{info.production}}</td>
                </tr>
                <tr>
                    <td>Cell Owner:</td>
                    <td>{{info.owner}}</td>
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
         production: this.selectedPoint.production,
         owner: this.selectedPoint.owner
       }

       return info
     }
   }
 }
</script>
