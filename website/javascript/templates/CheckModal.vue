<template>
  <div class="halite-modal" :class="{'on': isOn}">
    <div class="modal-overlay" @click="noCallback"></div>
    <div class="modal-container">
      <a @click="noCallback" class="close modal-close"><span class="icon-remove"></span></a>
      <h1 class="modal-title">{{ title_text }}</h1>
      <div class="halite-modal-body">
        <p>{{ body_text }}</p>
      </div>
      <div class="halite-modal-footer">
        <button type="button" @click="noCallback" class="btn" data-dismiss="modal"><span>{{ cancel_text }}</span></button>
        <button type="button" @click="yesCallback" class="btn"><span>{{ accept_text }}</span></button>
      </div>
    </div>
  </div>
</template>

<script>
  import vSelect from 'vue-select'
  import * as api from '../api'

export default {
  name: 'InputModal',
  props: ['isOn', 'noCallback', 'yesCallback', 'baseUrl', 'title_text', 'accept_text', 'cancel_text', 'body_text'],
  components: {vSelect},
  mounted: function() {},
  data: function() {
    return {}
  }
}
</script>
<style lang="scss" scoped>

</style>
