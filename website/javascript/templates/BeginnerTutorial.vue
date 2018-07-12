<template>
    <Walkthrough title="Beginner Tutorial" v-bind:steps="steps">
        Walkthrough!
    </Walkthrough>
</template>

<script>
    import Walkthrough from "./Walkthrough.vue";

    export default {
        name: "beginner-tutorial",
        components: {
            "Walkthrough": Walkthrough,
        },
        data: function () {
            return {
                steps: [
                    {
                        title: "The Seas of Halite",
                        description: "Description.",
                    },
                    {
                        title: "Your Fleet",
                        description: "This is your fleet.",
                    },
                ],
            };
        },
        mounted: function () {

        },
        methods: {

        },
    };
</script>

<style lang="scss" scoped>

</style>
