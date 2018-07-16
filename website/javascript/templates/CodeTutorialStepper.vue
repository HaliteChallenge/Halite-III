<template>
    <section>
        <BotEditor />
    </section>
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
    };
</script>

<style lang="scss" scoped>
    .body {
        /* Override inline style */
        margin-top: 0 !important;
    }
</style>
