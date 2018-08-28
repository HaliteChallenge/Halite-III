<template>
  <div class="main" role="main">
    <header class="navbar navbar-inverse unloaded">
      <div class="container-fluid">
        <div id="navbar" class="navbar-collapse collapse">
          <ul class="nav navbar-nav">
            <li><a v-on:click="save_current_file()"><span>Save</span></a></li>
            <li><a v-on:click="run_ondemand_game()"><span>Run</span></a></li>
            <li><a v-on:click="download_bot()"><span>Download</span></a></li>
            <li><a v-on:click="submit_bot()"><span>Submit</span></a></li>
          </ul>
          <ul class="nav navbar-nav halite-nav-right">
            <li>
              <a v-on:click="save_current_file">
                <span v-if="allSaved" class="saved">Saved</span>
                <span v-else>Not Saved</span>
                <i class="xline xline-bottom"></i>
              </a>
            </li>
            <li>
              <BotEditorSettings ref="settings_console" />
            </li>
          </ul>
        </div>
      </div>
    </header>
    <BotEditor ref="editor_pane" :baseUrl="baseUrl" @save="on_save" />
  </div>
</template>

<script>
  import * as api from '../api'
  import * as utils from '../utils'
  import BotEditor from './BotEditor.vue'
  import BotEditorSettings from './BotEditorSettings.vue'

  export default {
    name: 'bot_editor_page',
    props: ['baseUrl'],
    components: {BotEditor, BotEditorSettings},
    data: function () {
      return {
        allSaved: false
      }
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
      on_save(value) {
        this.allSaved = value
      }
    }
  }
</script>

<style lang="scss" scoped>
.main {
  height: 100%;
}

.halite-nav-right {
  float: right !important;
}

.saved {
  color: #009900;
}

#navbar {
  ul {
    li {
      a {
        padding: 0.25em 1em;
        min-height: 0;
      }
    }
  }
}

header.navbar {
  min-height: 0;
  height: auto;
}
</style>

<style lang="scss">
header.navbar {
  margin-bottom: 0 !important;
}

.body {
  margin-top: 0 !important;
}
</style>
<!--
     Local Variables:
     web-mode-style-padding: 0
     End:
     End: -->
