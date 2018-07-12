<!-----------------
     Resuable component to provide a sidebar with a list of steps &
     explanatory text (and possibly substeps), while indicating this
     information to a wrapped component.
   ------------------>
<template>
    <section class="walkthrough">
        <div class="walkthrough-nav">
            <h1>{{title}}</h1>
            <!-- TODO: this will have to get factored out -->
            <ul>
                <li
                    v-for="(step, index) in steps"
                    v-bind:class="{ active: index == progress }"
                    v-on:click="switchTo(index)"
                >
                    <h2>{{step.title}}</h2>
                    <p v-if="index == progress">
                        {{step.description}}
                    </p>
                </li>
            </ul>
        </div>

        <div class="walkthrough-content">
            <slot
                v-bind:progress="progress"
                v-bind:step-name="steps[progress].name"
            >
            </slot>
        </div>
    </section>
</template>

<script>
    export default {
        name: "walkthrough",
        props: ["title", "steps"],
        data: function () {
            return {
                progress: 0,
            };
        },
        mounted: function () {

        },
        methods: {
            switchTo: function(index) {
                this.progress = index;
            }
        },
    };
</script>

<style lang="scss" scoped>
    .walkthrough {
        display: flex;

        > .walkthrough-nav {
            flex: 1 0;
            border-right: 1px solid #474951;

            > ul {
                list-style-type: none;
                margin: 0;
                padding: 0;

                > li {
                    /* TODO: don't hardcode color */
                    border-top: 1px solid #474951;
                    color: #b9b8b8;
                    > h2 {
                        margin: 0;
                    }
                }

                > li.active {
                    > h2 {
                        color: #FFBE00;
                        background: rgba(255, 190, 0, 0.1);
                    }
                }

                > li:hover {
                    cursor: pointer;

                    > h2 {
                        background: rgba(255, 190, 0, 0.1);
                    }
                }
            }
        }

        > .walkthrough-content {
            flex: 3 0;
        }
    }
</style>
