<!-----------------
     A series of substeps in a walkthrough.
   ------------------>
<template>
    <section class="substeps">
        <h3>Tasks</h3>
        <ul>
            <slot></slot>
        </ul>
    </section>
</template>

<script>
    export default {
        name: "walkthrough-substeps",
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
