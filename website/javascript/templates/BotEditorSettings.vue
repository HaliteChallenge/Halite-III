<template>
  <el-popover
    placement="bottom"
    width="480"
    trigger="click"
    v-model="show"
    popper-class="tier-popover"
  >
    <a slot="reference" class='tier-clickable'>
      <span>Settings</span>
    </a>
    <div class="tier-description">
      <div class="header">
        <p class="header-text">Settings</p>
        <span class="close-btn icon-remove" @click="show = false" />
      </div>

      <h2>Editor</h2>
      <label for="expandTab">Tab inserts spaces:</label>
      <input type="checkbox" name="expandTab" @change="on_change" v-model="state.editor.expandTab" checked />

      <h2>Games</h2>

      <p><i>Opponent Bot</i></p>
      <select v-on:change="on_change" v-model="state.opponent">
          <option v-for="(name, id) in state.BOT_OPTIONS" v-bind:value="id">
            {{ name }}
          </option>
      </select>

      <p><i>Map Size</i></p>
      <select v-on:change="on_change" v-model="state.mapSize">
          <option v-for="size in MAP_OPTIONS" v-bind:value="size">
            {{ size }}
          </option>
      </select>
    </div>
  </el-popover>
</template>

<script>
  import Vue from 'vue'
  import {Popover} from 'element-ui'

  import editorState from '../editorState'

  export default {
    name: 'BotEditorSettings',
    components: {
      'el-popover': Popover
    },
    mounted: function () {},
    data: function () {
      return {
        state: editorState,
        show: false,
        MAP_OPTIONS: [32, 48, 64, 96, 128],
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
  .tier-clickable {
    cursor: pointer;
  }
  .tier-popover {
    background-color: rgb(36, 37, 44);
    border-color: rgb(36, 37, 44);
    padding: 10px 15px 15px;

    .popper__arrow {
      border-top-color: rgb(36, 37, 44) !important;
      border-bottom-color: rgb(36, 37, 44) !important;
      &::after {
        border-top-color: rgb(36, 37, 44) !important;
        border-bottom-color: rgb(36, 37, 44) !important;
      }
    }
  }

  .tier-description {

    color: #FFFFFF;
    text-align: left;

    p {
      margin: 5px 0;
      font-size: 14px;
    }

    .header {
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    .header-text {
      margin: 0;
      text-transform: Uppercase;
      color: rgb(99, 205, 202);
      font-size: 24px;
      letter-spacing: 1px;
      font-family: Teko !important;
    }

    .close-btn {
      cursor: pointer;
    }

    .tier-more-btn {
      margin-top: 10px;
    }
  }

  span {
    display: block;
  }
  span a {
    display: block;
    text-decoration: none;
  }
  select {
    width: 100%;
    height: 37px;
    padding: 5px 16px;
    background-color: #fff;
    border: 1px solid #ccc;
    border-radius: 3px;
    color: #30242F;
  }
</style>
<!--
     Local Variables:
     web-mode-script-padding: 2
     End:
     End: -->
