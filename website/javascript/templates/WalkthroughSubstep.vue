<!-----------------
     A substep in a walkthrough.
   ------------------>
<template>
    <li>
        <input type="checkbox" disabled />
        <slot></slot>
    </li>
</template>

<script>
    export default {
        name: "walkthrough-substep",
        props: [],
        data: function() {
            return {
            };
        },
        computed: {
        }
    };
</script>

<style lang="scss" scoped>
</style>
