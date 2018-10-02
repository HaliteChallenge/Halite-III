<template>
    <section class="fixed pin bg-translucent">
        <section class="rounded m-8 px-4 pt-4 bg-gradient shadow-lg">
            <header>
                <slot name="header"></slot>
            </header>
            <slot name="content"></slot>
            <nav class="modal-buttons -mr-4 mt-4 flex justify-end">
                <slot name="buttons"></slot>
            </nav>
        </section>
    </section>
</template>

<script>
    export default {
        data() {
            return {};
        },
    }
</script>

<style lang="scss">
    /* .modal-container {
       display: flex;
       position: fixed;
       top: 0;
       bottom: 0;
       left: 0;
       right: 0;

       background: rgba(0, 0, 0, 0.5);

       justify-content: center;
       align-items: center;

       .modal {
       background: #FFF;
       width: 80vw;
       min-width: 30em;
       padding: 1em;
       box-shadow: 0 2px 5px rgba(0, 0, 0, 0.5);

       >nav {
       display: flex;
       margin: 0 -1em -1em;
       justify-content: flex-end;

       button {
       flex: 1 0;
       max-width: 20em;
       margin: 0;
       }
       }
       }
       } */
</style>
