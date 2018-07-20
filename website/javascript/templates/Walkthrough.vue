<!-----------------
     Resuable component to provide a sidebar with a list of steps &
     explanatory text (and possibly substeps), while indicating this
     information to a wrapped component.
   ------------------>
<!--
<template>
    <section class="walkthrough">
        <div class="walkthrough-nav">
            <h1>{{title}}</h1>
            <section class="steps" ref="stepsContainer">
                <slot
                    v-if="steps[progress]"
                    name="steps"
                    v-bind:activeStep="steps[progress].name"
                >
                </slot>
            </section>
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
                v-if="steps[progress]"
                name="content"
                v-bind:progress="progress"
                v-bind:step-name="steps[progress].name"
            >
            </slot>
        </div>
    </section>
</template>
-->

<script>
    export default {
        name: "walkthrough",
        props: ["title"],
        data: function() {
            return {
                progress: 0,
                steps: [],
                substeps: {},
            };
        },
        provide() {
            return {
                substeps: this.substeps,
                completeSubstep: (substep) => {
                    this.$set(this.substeps, substep, true);
                },
            };
        },
        created: function() {
            let i = 0;
            for (const node of this.$slots.steps) {
                if (node.tag && node.tag.endsWith("walkthrough-step")) {
                    i += 1;
                    const step = Object.assign({}, node.componentOptions.propsData);
                    step.body = node.componentOptions.children;
                    this.steps.push(step);
                }
            }
        },
        mounted: function() {
            if (window.history.state && history.state.progress) {
                this.switchTo(history.state.progress, false);
            }
            else {
                this.switchTo(0, false);
            }
            // Make sure we overwrite data once in render func
            this.$forceUpdate();

            window.addEventListener("popstate", (e) => {
                if (e.state && typeof e.state.progress !== "undefined") {
                    this.switchTo(e.state.progress, false);
                }
            });
        },
        render: function(h) {
            return h("section", {
                class: "walkthrough",
            }, [
                h("section", {
                    class: "walkthrough-nav"
                }, [
                    h("h1", this.title),
                    h("section", {
                        class: "steps",
                        ref: "stepsContainer",
                    }, this.$slots.steps
                           .filter((node) => node.tag && node.tag.endsWith("walkthrough-step"))
                           .map((step, index) => {
                               if (step.child) {
                                   step.child.active = index === this.progress;
                                   step.child.switchTo = () => this.switchTo(index);
                               }
                               return step;
                           })),
                    h("nav", [
                        h("button", {
                            on: {
                                click: this.prevStep,
                            },
                        }, "Back"),
                        h("button", {
                            on: {
                                click: this.nextStep,
                            },
                        }, "Next"),
                    ])
                ]),

                h("section", {
                    class: "walkthrough-content"
                }, [
                    this.$scopedSlots.content({
                        progress: this.progress,
                        stepName: this.steps[this.progress].name,
                    })
                ]),
            ]);
        },
        methods: {
            switchTo: function(index, addState=true) {
                this.progress = Math.min(this.steps.length - 1, Math.max(0, index));
                if (addState) {
                    window.history.pushState({
                        progress: this.progress,
                    }, this.steps[this.progress], `#tutorial-${this.progress}`);
                }

                const container = this.$refs.stepsContainer;
                // TODO: don't hardcode 40 (=2.5em)
                container.scrollTop = 40 * this.progress;
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
            min-width: 20em;
            max-width: 25em;

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

            > .steps {
                flex: 1 0;
                list-style-type: none;
                overflow-y: scroll;
                margin: 0;
                padding: 0;
            }
        }

        > .walkthrough-content {
            flex: 2 0;
        }
    }
</style>
