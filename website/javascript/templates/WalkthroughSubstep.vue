<!-----------------
     A substep in a walkthrough.
   ------------------>
<template>
    <li>
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
</style>
