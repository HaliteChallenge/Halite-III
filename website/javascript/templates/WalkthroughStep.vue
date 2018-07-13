<!-----------------
     A single step in a walkthrough.
   ------------------>
<template>
    <section
        class="step"
        v-bind:class="{ active }"
        v-on:click="switchTo"
    >
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
                switchTo: () => {},
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
        height: 2em;
        overflow: hidden;

        > h2 {
            margin: 0;
            transition: 0.3s all ease-in-out;
        }
    }

    .step.active {
        height: auto;
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
