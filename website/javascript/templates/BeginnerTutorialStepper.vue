<template>
    <section :class="`step-${stepName}`">
        <Visualizer
            ref="visualizer"
            v-if="replay"
            v-bind:replay="replay"
            v-bind:game="game"
            :dashboard="false"
            :width="500"
            :height="500"
            :autoplay="false"
        />
    </section>
</template>

<script>
    import * as libhaliteviz from '../../../libhaliteviz'
    import Visualizer from "./Visualizer.vue";

    export default {
        name: "beginner-tutorial-stepper",
        props: ["progress", "step-name"],
        inject: ["completeSubstep"],
        components: {Visualizer},
        data: function () {
            return {
                replay: null,
                game: null,
            };
        },
        mounted: function() {
            window.fetch("/assets/replay.hlt")
                  .then((req) => req.arrayBuffer())
                  .then((buf) => libhaliteviz.parseReplay(buf))
                  .then((replay) => {
                      this.replay = replay;
                  });
        },
        methods: {
        },
        updated: function() {
            if (!this.boundEvents) {
                this.$nextTick(function() {
                    if (this.$refs.visualizer) {
                        const visualizer = this.$refs.visualizer.getVisualizer();
                        const patch = function(obj, prop, cb) {
                            const orig = obj[prop];
                            obj[prop] = function(...args) {
                                cb(...args);
                                return orig.apply(this, args);
                            }.bind(obj);
                        };

                        patch(visualizer.camera, "panByPixel", () => {
                            this.completeSubstep("seas-pan");
                        });
                        patch(visualizer.camera, "zoomBy", () => {
                            this.completeSubstep("seas-zoom");
                        });
                        patch(visualizer.camera, "reset", () => {
                            this.completeSubstep("seas-reset");
                        });
                        patch(visualizer, "onSelect", (kind) => {
                            if (this.stepName === "salt" && kind === "point") {
                                this.completeSubstep("salt-halite");
                            }
                        });

                        this.boundEvents = true;
                    }
                });
            }
        },
        watch: {
        },
    };
</script>

<style lang="scss" scoped>
    section {
        color: #000;
    }
</style>

<style lang="scss">
    .game-replay-share {
        display: none;
    }

    .step-seas, .step-time, .step-salt {
        .game-replay-progress, .replay-btn:not(.reset-btn) {
            display: none;
        }
    }

    .step-seas, .step-time {
        .sidebar {
            display: none;
        }

        canvas {
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.5);
        }
    }

    .game-heading, .xline {
        display: none;
    }

    .game-replay, .game-replay-viewer, .visuallizer-container {
        padding: 0 !important;
    }

    .visuallizer-container {
        height: 100%;
    }

    .visuallizer-container > .row {
        display: flex;
        justify-content: center;
        align-items: center;
        height: 100%;
    }

    .visuallizer-container > .row > .col-md-8 {
        width: 100%;
        flex: 1 0;
    }

    .walkthrough-content {
        height: 100%;
        & > * {
            height: 100%;
        }
    }
</style>
