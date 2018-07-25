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
                if (newStep === "initialization") {
                    this.$refs.editor
                        .doReady((editor) => {
                            editor.clearHighlights();
                            editor.highlightContaining("MyPythonBot");
                        });
                }
                else if (newStep === "game-loop") {
                    this.$refs.editor
                        .doReady((editor) => {
                            editor.clearHighlights();
                            editor.highlightContaining(/(while True)|(get_frame)|(current game state)/);
                        });
                }
                else if (newStep === "basic-movement") {
                    this.$refs.editor
                        .doReady((editor) => {
                            editor.clearHighlights();
                            editor.highlightContaining(/(commands\.move)|(Move in)|(for ship)|(our ships)/);
                            editor.highlightContaining("end_turn", "tutorial-highlight-alt");
                        });
                }
                else if (newStep === "carrier") {
                    this.$refs.editor
                        .doReady((editor) => {
                            editor.clearHighlights();
                            editor.highlightContaining("commands.spawn", "tutorial-highlight");
                        });
                }
                else {
                    this.$refs.editor
                        .doReady((editor) => {
                            editor.clearHighlights();
                        });
                }
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
