<template>
    <section>
        <p>{{progress}}</p>
        <p>{{stepName}}</p>
    </section>
</template>

<script>
    export default {
        name: "beginner-tutorial-stepper",
        props: ["progress", "step-name"],
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

</style>
