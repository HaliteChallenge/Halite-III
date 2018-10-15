<template>
  <a class="editor-settings-button" v-if="!show" @click="show=true">
    <span>Settings</span>
  </a>
  <div class="editor-settings-popover" v-else>
    <div class="editor-settings">
      <div class="header">
        <h2>Settings</h2>
        <span class="close-btn icon-remove" @click="show = false" />
      </div>

      <h3>Editor</h3>
      <div>
        <label for="expandTab">Tab inserts spaces:</label>
        <input type="checkbox" name="expandTab" @change="on_change" v-model="state.editor.expandTab" checked />
      </div>

      <button class="btn-sm btn-primary" @click="$emit('deleteAll')">Delete Bot and Reset Language</button>

      <h3>Games</h3>

      <div>
        <label for="opponent-bot">Opponent Bot</label>
        <select id="opponent-bot" v-on:change="on_change" v-model="state.opponent">
          <option v-for="(name, id) in state.BOT_OPTIONS" v-bind:value="id">
            {{ name }}
          </option>
        </select>
      </div>

      <div>
        <label for="map-size">Map Size</label>
        <select id="map-size" v-on:change="on_change" v-model="state.mapSize">
          <option v-for="size in MAP_OPTIONS" v-bind:value="size">
            {{ size }}
          </option>
        </select>
      </div>
    </div>
  </div>
</template>

<script>
  import Vue from 'vue'

  import editorState from '../editorState'

  export default {
    name: 'BotEditorSettings',
    mounted: function () {},
    data: function () {
      return {
        state: editorState,
        show: false,
        MAP_OPTIONS: [32, 40, 48, 56, 64],
      }
    },
    methods: {
      on_change: function() {
        this.$emit('settings_change')
      },
    }
  }
</script>

<style lang="scss" scoped>
    .editor-settings-popover {
      position: fixed;
      top: 0;
      bottom: 0;
      left: 0;
      right: 0;
      background: rgba(0, 0, 0, 0.5);
      display: flex;
      justify-content: center;
      align-items: center;
      z-index: 99999;
    }

    select {
       height: 37px;
       padding: 5px 16px;
       background-color: #fff;
       border: 1px solid #ccc;
       border-radius: 3px;
       color: #52678D;
       margin-left: 10px;
    }

    .editor-settings {
      flex: 0 0;
      min-width: 50em;
      height: 25em;
      background-image: linear-gradient(0deg, #002D77 0%, #003A99 100%);
      padding: 1em;

      .header {
        h2 {
          margin: 0;
          display: inline-block;
        }

        .close-btn {
          float: right;
          cursor: pointer;
        }
      }

      label {
        font-size: 16px;
      }
    }

</style>
<!--
     Local Variables:
     web-mode-script-padding: 2
     End:
     End: -->
