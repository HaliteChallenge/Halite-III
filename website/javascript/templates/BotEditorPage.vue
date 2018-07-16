<template>
  <div class="main" role="main">
    <header class="navbar navbar-inverse navbar-fixed-top unloaded">
      <div class="container-fluid">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navbar" aria-expanded="false" aria-controls="navbar">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
          <a class="navbar-brand" href="/">
            <img alt="Halite" src="/assets/images/full_logo.svg">
          </a>
        </div>
        <div id="navbar" class="navbar-collapse collapse">
          <ul class="nav navbar-nav">
            <li><a v-on:click="save_current_file()"><span>Save</span><i class="xline xline-bottom"></i></a></li>
            <li><a v-on:click="run_ondemand_game()"><span>Run</span><i class="xline xline-bottom"></i></a></li>
            <li><a v-on:click="download_bot()"><span>Download</span><i class="xline xline-bottom"></i></a></li>
            <li><a v-on:click="submit_bot()"><span>Submit</span><i class="xline xline-bottom"></i></a></li>
          </ul>
        </div>
        <div id="user-profile-bar-container"></div>
      </div>
    </header>
    <BotEditor ref="editor_pane"/>
  </div>
</template>

<script>
  import * as api from '../api'
  import * as utils from '../utils'
  import * as libhaliteviz from '../../../libhaliteviz'
  import BotEditor from './BotEditor.vue'

export default {
  name: 'bot_editor_page',
  data: function () {
    return {}
  },
   components: {
    BotEditor
  },
  mounted: function () {
    utils.initUserProfileNav();
  },
  methods: {
    download_bot: function () {
      this.$refs.editor_pane.download_bot();
    },
    run_ondemand_game: function() {
      this.$refs.editor_pane.run_ondemand_game();
    },
    save_current_file: function() {
      this.$refs.editor_pane.save_current_file();
    },
    submit_bot: function () {
      this.$refs.editor_pane.submit_bot();
    },
  }
}
</script>

<style lang="scss" scoped>
.main {
 height: 100%;
}

</style>
