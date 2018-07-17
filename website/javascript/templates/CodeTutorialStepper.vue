<template>
    <BotEditor ref="editor" v-bind:tutorial="true" />
</template>

<script>
    import BotEditor from "./BotEditor.vue";
    export default {
        name: "code-tutorial-stepper",
        props: ["progress", "step-name"],
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

        },
        watch: {
            stepName: function(newStep, oldStep) {
                if (newStep === "initialization") {
                    this.$refs.editor
                        .doReady((editor) => editor.highlightContaining("MyPythonBot"));
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
