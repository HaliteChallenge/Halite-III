<template>
  <div class="panel panel-stats">
    <div class="panel-heading" role="tab" id="heading_player_details">
      <a data-toggle="collapse" v-on:click="emitClick" @click.stop="togglePanel" data-parent="#accordion" :aria-expanded="show.toString()" aria-controls="widget_player_details">
        <h4>{{title}}</h4>
        <span class="toggle-icon chevron"></span>
      </a>
    </div>
    <div class="panel-collapse collapse" :class="{'in': show}" role="tabpanel" :aria-expanded="show.toString()" id="widget_player_details" aria-labelledby="heading_player_details">
      <slot v-bind:panelOpen="show"></slot>
    </div>
  </div>
</template>

<script>
  export default {
    props: {
      name: {
        required: true,
      },
      title: {
        required: true,
      },
    },
    data() {
      return {
        show: window.sessionStorage.getItem(`halite-showpanel-${this.name}`) === 'true',
      }
    },
    methods: {
      emitClick() {
        this.$emit('click')
      },
      togglePanel() {
        this.show = !this.show
        window.sessionStorage.setItem(`halite-showpanel-${this.name}`, this.show.toString())
      },
      open() {
        this.show = true
        window.sessionStorage.setItem(`halite-showpanel-${this.name}`, this.show.toString())
      },
    },
  }
</script>
