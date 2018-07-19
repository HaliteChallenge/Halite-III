<!-----------------
     A substep in a walkthrough.
   ------------------>
<template>
    <li v-bind:class="{ checked: substeps[this.name] }">
        <input
            type="checkbox"
            disabled
            v-bind:checked="substeps[this.name]"
        />
        <slot></slot>
    </li>
</template>

<script>
    export default {
        name: "walkthrough-substep",
        props: ["name"],
        inject: ["substeps"],
        data: function() {
            return {
            };
        },
        computed: {
        },
    };
</script>

<style lang="scss" scoped>
    @keyframes highlight {
        0% {
            background-color: rgba(255, 255, 255, 0);
        }

        50% {
            background-color: rgba(255, 255, 255, 1);
        }

        100% {
            background-color: rgba(255, 255, 255, 0);
        }
    }

    li.checked {
        animation: highlight 2s alternate;
    }
</style>
