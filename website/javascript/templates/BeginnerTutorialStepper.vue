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
    import * as tween from "../tween";
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
            Promise.all([
                import(/* webpackChunkName: "libhaliteviz" */ "libhaliteviz"),
                window.fetch("/assets/replay.hlt"),
            ]).then(([ libhaliteviz, req ]) => Promise.all([
                Promise.resolve(libhaliteviz),
                req.arrayBuffer(),
            ])).then(([ libhaliteviz, buf ]) =>
                libhaliteviz.setAssetRoot("")
                            .then(() => libhaliteviz.parseReplay(buf))
            ).then((replay) => {
                this.replay = replay;
            });
        },
        methods: {
            updateVisibility: function(stepName) {
                if (!this.$refs.visualizer) {
                    return;
                }
                const visualizer = this.$refs.visualizer.getVisualizer();
                visualizer.factoryContainer.visible =
                    stepName !== "seas" &&
                    stepName !== "time" &&
                    stepName !== "salt" &&
                    stepName !== "admiral";
                visualizer.entityContainer.visible =
                    stepName !== "seas" &&
                    stepName !== "time" &&
                    stepName !== "salt";
                visualizer.render();

                if (stepName === "admiral") {
                    // Scrub to frame 1 so ships actually show
                    visualizer.scrub(1, 0);
                    tween.driveWhile(
                        () => visualizer.camera.scale < 40,
                        (dt) => visualizer.camera.zoomBy(0.5, 0.5, dt / 10000)
                    )
                         .then(() => tween.driveWhile(
                             () => (visualizer.camera.pan.x !== 0 ||
                                    visualizer.camera.pan.y !== 22),
                             (dt) => {
                                 if (visualizer.camera.pan.x !== 0) {
                                     visualizer.camera.panByPixel(dt / 1000, 0);
                                 }
                                 if (visualizer.camera.pan.y !== 22) {
                                     visualizer.camera.panByPixel(0, dt / 1000);
                                 }
                             }
                         ))
                }
                else if (stepName === "collection") {
                    tween.driveWhile(
                        () => visualizer.camera.scale > visualizer.camera.initScale,
                        (dt) => visualizer.camera.zoomBy(0.5, 0.5, -dt / 10000)
                    ).then(() => visualizer.camera.reset());
                }
            },
        },
        updated: function() {
            if (!this.boundEvents) {
                const bindEvents = () => {
                    if (this.$refs.visualizer && this.$refs.visualizer.getVisualizer) {
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
                        patch(visualizer, "play", () => {
                            this.completeSubstep("collection-play");
                        });
                        patch(visualizer.onSelect, "dispatch", (kind) => {
                            if (this.stepName === "salt" && kind === "point") {
                                this.completeSubstep("salt-halite");
                            }
                            else if (this.stepName === "vessel" && kind === "ship") {
                                this.completeSubstep("vessel-halite");
                            }
                        });

                        this.updateVisibility(this.stepName);

                        this.boundEvents = true;
                    }
                    else {
                        window.setTimeout(() => {
                            bindEvents();
                        }, 500);
                    }
                };
                this.$nextTick(function() {
                    bindEvents();
                });
            }
        },
        watch: {
            stepName: function(newValue) {
                this.updateVisibility(newValue);
            },
        },
    };
</script>

<style lang="scss" scoped>
    section {
        color: #000;
    }
</style>

<style lang="scss">
    .walkthrough {
        .game-replay-share {
            display: none;
        }

        .step-seas {
            .game-replay-progress {
                display: none;
            }
        }

        .step-time {
            .game-replay-progress {
                animation: reveal 4s;
            }
        }

        .step-seas, .step-time, .step-salt, .step-admiral, .step-shipyard, .step-vessel {
            .replay-btn:not(.reset-btn) {
                display: none;
            }
        }

        .step-seas, .step-time {
            .sidebar {
                display: none;
            }

            .visuallizer-container > .row > .col-md-8 {
                width: 100%;
            }

            canvas {
                box-shadow: 0 2px 5px rgba(0, 0, 0, 0.5);
            }
        }

        .game-heading, #heading_player_details, .game-stats-widget {
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
            width: calc(100% - 2em);
            margin: 0 auto;
        }

        .visuallizer-container > .row > .col-md-8 {
            flex: 1 0;
        }

        .walkthrough-content {
            height: 100%;
            & > * {
                height: 100%;
            }
        }
    }

    @keyframes reveal {
        0% {
            opacity: 0;
            background-color: rgba(255, 255, 255, 0);
        }

        25% {
            opacity: 1;
            background-color: rgba(255, 255, 255, 0);
        }

        50% {
            background-color: rgba(255, 255, 255, 1);
        }

        75% {
            background-color: rgba(255, 255, 255, 0);
        }

        100% {
            background-color: rgba(255, 255, 255, 1);
        }
    }
</style>
