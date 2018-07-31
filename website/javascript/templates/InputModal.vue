<template>
  <div class="halite-modal" :class="{'on': isOn}">
    <div class="modal-overlay" @click="closeCallback"></div>
    <div class="modal-container">
      <a @click="closeCallback" class="close modal-close"><span class="icon-remove"></span></a>
      <h1 class="modal-title">{{ title_text }}</h1>
      <div class="alert alert-danger" :class="{'hidden': is_name_valid}" role="alert">
        <strong>Oh snap!</strong> Change a few things up and try submitting again.
      </div>
      <div class="halite-modal-body">
        <input v-model="file_name" class="halite-modal-input" placeholder="file name">
      </div>
      <div class="halite-modal-footer">
        <button type="button" @click="closeCallback" class="btn" data-dismiss="modal"><span>{{ cancel_text }}</span></button>
        <button type="button" @click="create_file" class="btn"><span>{{ accept_text }}</span></button>
      </div>
    </div>
  </div>
</template>

<script>
  import vSelect from 'vue-select'
  import * as api from '../api'

export default {
  name: 'InputModal',
  props: ['isOn', 'closeCallback', 'createCallback', 'baseUrl', 'title_text', 'accept_text', 'cancel_text'],
  components: {vSelect},
  mounted: function() {},
  data: function() {
    return {
      is_name_valid: true,
      file_name: ""
    }
  },
  watch: {
    file_name: function(value){
      this.is_name_valid = (!/[^a-z0-9_.@()-]/i.test(value))
      console.log(this.is_name_valid)
    },
    isOn: function(value) {
      if(value) {
        this.file_name = ""
      }
    }
  },
  methods: {
    create_file: function(){
      if(this.is_name_valid) {
        this.createCallback(this.file_name)
      }
    }
  }
}
</script>
<style lang="scss" scoped>

</style>
