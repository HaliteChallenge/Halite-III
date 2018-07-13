<!-----------------
     A single step in a walkthrough.
   ------------------>
<template>
    <section class="step" v-bind:class="{ active }">
        <h2>{{title}}</h2>
        <slot></slot>
    </section>
</template>

<script>
    export default {
        name: "walkthrough-step",
        props: ["title", "name"],
        data: function() {
            return {
                active: false,
            };
        },
        computed: {
        }
    };
</script>

<style lang="scss" scoped>
    .step {
        /* TODO: don't hardcode color */
        border-top: 1px solid #474951;
        color: #b9b8b8;
        max-height: 2em;
        overflow: hidden;

        > h2 {
            margin: 0;
            transition: 0.3s all ease-in-out;
        }
    }

    .step.active {
        max-height: 10em;

        > h2 {
            color: #FFBE00;
            background: rgba(255, 190, 0, 0.1);
        }
    }

    .step:hover {
        cursor: pointer;

        > h2 {
            background: rgba(255, 190, 0, 0.1);
        }
    }
</style>
