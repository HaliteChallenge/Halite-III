<template>
  <div class="halite-modal" :class="{'on': isOn}">
    <div class="modal-overlay"></div>
    <div class="modal-container">
      <h1 class="modal-title">Pick a Bot Language</h1>
      <div class="halite-modal-body">
        <button
          v-for="language in languages"
          type="button"
          @click="pickLanguage(language)"
          class="btn"
          data-dismiss="modal"
        ><span>{{language}}</span></button>
      </div>
      <div class="halite-modal-footer">
      </div>
    </div>
  </div>
</template>

<script>
  import * as api from '../api'

  export default {
    name: 'InputModal',
    props: ['isOn', 'tutorial'],
    mounted: function() {
    },
    data: function() {
      return {
        languages: this.tutorial ? ["Python3"] : ["Python3", "Java", "C++"]
      }
    },
    watch: {
    },
    methods: {
      pickLanguage: function(language) {
        this.$emit("choose", language)
      }
    }
  }
</script>
<style lang="scss" scoped>

</style>
