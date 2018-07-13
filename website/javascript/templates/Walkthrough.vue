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
            <ul ref="stepsContainer">
                <li
                    v-for="(step, index) in steps"
                    v-bind:class="{ active: index == progress }"
                    v-on:click="switchTo(index)"
                >
                    <h2>{{step.title}}</h2>
                    <p>
                        {{step.description}}
                    </p>
                </li>
            </ul>
            <nav>
                <button
                    v-on:click="prevStep"
                >
                    Back
                </button>
                <button
                    v-on:click="nextStep"
                >
                    Next
                </button>
            </nav>
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
            if (window.history.state && history.state.progress) {
                this.switchTo(history.state.progress, false);
            }
            else {
                this.switchTo(0, false);
            }

            window.addEventListener("popstate", (e) => {
                if (e.state && typeof e.state.progress !== "undefined") {
                    this.switchTo(e.state.progress, false);
                }
            });
        },
        methods: {
            switchTo: function(index, addState=true) {
                this.progress = Math.min(this.steps.length, Math.max(0, index));
                if (addState) {
                    window.history.pushState({
                        progress: this.progress,
                    }, this.steps[this.progress], `#tutorial-${this.progress}`);
                }

                const container = this.$refs.stepsContainer;
                // TODO: don't hardcode 32 (=2em)
                container.scrollTop = 32 * this.progress;
            },
            nextStep: function() {
                this.switchTo(this.progress + 1);
            },
            prevStep: function() {
                this.switchTo(this.progress - 1);
            },
        },
    };
</script>

<style lang="scss" scoped>
    .walkthrough {
        display: flex;

        > .walkthrough-nav {
            display: flex;
            flex-direction: column;
            flex: 1 0;
            border-right: 1px solid #474951;
            max-width: 15em;

            > h1 {
                margin: 0;
                padding: 0.25em 0;
                flex: 0 0;
            }

            > nav {
                flex: 0 1 2em;
                display: flex;
                flex-direction: row;

                button {
                    flex: 1;
                    color: #000;
                }
            }

            > ul {
                flex: 1 0;
                list-style-type: none;
                overflow-y: scroll;
                margin: 0;
                padding: 0;

                > li {
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

                > li.active {
                    max-height: 10em;

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
