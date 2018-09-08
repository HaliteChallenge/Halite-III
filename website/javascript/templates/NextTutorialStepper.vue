<template>
  <BotEditor ref="editor" v-bind:tutorial="true" v-bind:baseUrl="baseUrl" />
</template>

<script>
  import BotEditor from "./BotEditor.vue";
  export default {
    name: "code-tutorial-stepper",
    props: ["progress", "step-name", "baseUrl"],
    components: {
      "BotEditor": BotEditor,
    },
    data: function () {
      return {
      };
    },
    mounted: function () {
    },
    methods: {
      runGame(params) {
        return this.$refs.editor.run_ondemand_game(params);
      },
      upload() {
        return this.$refs.editor.submit_bot();
      }
    },
    watch: {
      stepName: function(newStep, oldStep) {
        this.$refs.editor
            .doReady((editor) => {
              editor.clearHighlights();
            });
      },
    }
  };
</script>

<style lang="scss" scoped>
    .body {
      /* Override inline style */
      margin-top: 0 !important;
      height: 100%;
    }
</style>
