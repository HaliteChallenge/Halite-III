const PIXI = require("pixi.js");
const $ = require("jquery");
const extraFilters = require("pixi-extra-filters");
const GlowFilter = extraFilters.GlowFilter;
const pako = require("pako");

import {Ship} from "./ship";
import {playerSprite} from "./sprite";
import {Planet, holidaySprite} from "./planet";
import {Factory} from "./factory";
import {Fish} from "./fish";
import {Map} from "./map";
import * as statistics from "./statistics";
import * as keyboard from "./keyboardControls";

import * as assets from "./assets";

import * as animation from "./animation";

export const isHoliday = holidaySprite;

export class HaliteVisualizer {
    constructor(replay) {
        this.replay = replay;
        this.map_width = replay.production_map.width;
        this.map_height = replay.production_map.height;
        //this.stats = new statistics.Statistics(replay);

        this.frame = 0;
        this.time = 0;
        this._playing = false;

        this.timeStep = 0.01;
        this.playSpeed = 2.0;
        this.scrubSpeed = 0.25;
        // Keyboard controls - map a key name to an action, or directly to
        // a handler
        this.keyboardControls = new keyboard.KeyboardControls(this, {
            "ArrowLeft": "scrubBackwards",
            "KeyA": "scrubBackwards",
            "ArrowRight": "scrubForwards",
            "KeyD": "scrubForwards",
            "Space": () => {
                if (this.isPlaying()) this.pause();
                else this.play();
            },
        });

        this.application = new PIXI.Application(
            assets.VISUALIZER_SIZE,
            assets.VISUALIZER_HEIGHT,
            {
                backgroundColor: 0x222222,
            }
        );
        // Seems to help with pixelation when downscaling
        // Also: make sure textures have power-of-2 dimensions
        this.application.renderer.roundPixels = true;

        // Preload assets to avoid a hang when they're used for the first time
        // (since for large textures, image decode and GPU upload take a while)
        assets.prepareAll(this.application.renderer, this.application.renderer.plugins.prepare);

        // Scale things to fit exactly in the visible area
        this.scale = assets.VISUALIZER_HEIGHT / (this.map_height * assets.CELL_SIZE);
        if (this.map_width * this.scale * assets.CELL_SIZE > assets.VISUALIZER_SIZE) {
            this.scale = assets.VISUALIZER_SIZE / (this.map_width * assets.CELL_SIZE);
        }

        this.container = new PIXI.Container();

        this.container.position.x = 0;
        this.container.position.y = 0;
        this.letterbox = new PIXI.Graphics();
        if (this.container.position.y > 0) {
            this.letterbox.beginFill(0xFFFFFF);
            this.letterbox.drawRect(0, 0, assets.VISUALIZER_SIZE, this.container.position.y);
            this.letterbox.drawRect(
                0,
                this.container.position.y + this.map_height * this.scale * assets.CELL_SIZE,
                assets.VISUALIZER_SIZE,
                this.container.position.y);
        }
        if (this.container.position.x > 0) {
            this.letterbox.beginFill(0xFFFFFF);
            this.letterbox.drawRect(0, 0, this.container.position.x, assets.VISUALIZER_HEIGHT);
            this.letterbox.drawRect(
                this.container.position.x + this.map_width * this.scale * assets.CELL_SIZE,
                0,
                this.container.position.x,
                assets.VISUALIZER_HEIGHT);
        }


        this.mapContainer = new PIXI.Container();
        this.factoryContainer = new PIXI.Container();
        this.entityContainer = new PIXI.Container();
        this.fishContainer = new PIXI.Container();
        // this.dockingContainer = new PIXI.Container();
        // this.overlay = new PIXI.Graphics();
        this.lights = new PIXI.Graphics();
        this.lights.blendMode = PIXI.BLEND_MODES.SCREEN;
        this.lights.filters = [new GlowFilter(20, 1.5, 0.5, 0xFFFFFF, 0.3)];

        this.entities = [];
        this.factories = [];
        this.fish = [];
        this.player_energies = {};
        for (let i = 0; i < this.replay.players.length; i++) {
            const factoryBase = {"x" : this.replay.players[i].factory_location[0], "y" : this.replay.players[i].factory_location[1], "owner" : this.replay.players[i].player_id };
            const factory = new Factory(factoryBase, this.replay.constants,
                this.scale, (kind, args) => this.onSelect(kind, args), this.application.renderer);
            this.factories.push(factory);
            factory.attach(this.factoryContainer);
            // Add players'initial entities
            for (let entity_json of this.replay.players[i].entities) {
                let entity_object = {"x" : entity_json.x, "y" : entity_json.y, "energy" : entity_json.energy, "owner": this.replay.players[i].player_id};
                let new_entity = new playerSprite(this, entity_object);
                this.entities.push(new_entity);
                new_entity.attach(this.entityContainer);
            }

            // also a swarm of fish!
            // const fish = new Fish(this.replay.constants, this.scale, (kind, args) => this.onSelect(kind, args));
            // this.fish.push(fish);
            // fish.attach(this.fishContainer);
        }

        this.baseMap = new Map(this.replay, this.replay.GAME_CONSTANTS, this.scale,
            (kind, args) => this.onSelect(kind, args), this.application.renderer);
        this.baseMap.attach(this.mapContainer);


        // Prerender the points of interest once, and keep it as a texture

        this.container.addChild(this.mapContainer);
        // // this.container.addChild(this.dockingContainer);
        this.container.addChild(this.factoryContainer);
        this.container.addChild(this.entityContainer);
        this.container.addChild(this.fishContainer);
        //this.container.addChild(this.overlay);
        //this.container.addChild(this.lights);

        this.application.stage.addChild(this.container);
        this.application.stage.addChild(this.letterbox);

        this.timer = null;

        this.animationQueue = [];
        // Keep track of when ships die this frame, so that we don't
        // draw them after they die
        this.deathFlags = {};

        this.onUpdate = function() {};
        this.onPlay = function() {};
        this.onPause = function() {};
        this.onEnd = function() {};
        this.onSelect = function() {};
        this.onDeselect = function() {};

        this._onKeyUp = null;
        this._onKeyDown = null;

        this.application.render();
    }

    /**
     * Clean up the visualizer and stop it from rendering.
     *
     * Call this before creating a new instance, or you'll wonder why
     * everything is so slow!
     */
    destroy() {
        this.keyboardControls.destroy();
        this.keyboardControls = null;
        this.pause();
        this.application.destroy(true);
        PIXI.ticker.shared.stop();
        // TODO: destroy ships, planets
        // TODO: profile CPU, memory usage to make sure all is well
    }

    encodeVideo(type="canvas") {
        console.log("Encoding video");
        if (!window.RecordRTC) {
            const error = "RecordRTC.js not loaded!";
            console.error(error);
            return Promise.error(error);
        }
        this.pause();

        this.frame = 0;
        this.time = 0;
        const recorder = RecordRTC(this.application.renderer.view, {
            type: type,
        });
        recorder.startRecording();
        this.play();
        const oldEnd = this.onEnd;

        return new Promise((resolve) => {
            this.onEnd = () => {
                recorder.stopRecording(() => {
                    const blob = recorder.getBlob();
                    console.log(blob);

                    resolve(blob);
                });
                this.onEnd = oldEnd;
            };
        });
    }

    encodeGIF(start, stop, resolution=10) {
        if (!window.GIF) {
            const error = "GIF.js not loaded, can't encode GIF";
            console.error(error);
            return Promise.error(error);
        }

        this.pause();
        PIXI.ticker.shared.stop();
        const gif = new GIF({
            workers: 2,
            quality: 2,
            // TODO:
            workerScript: "assets/js/gif.worker.js",
        });

        this.frame = start.frame;
        this.time = 0;

        const timestep = 1.0 / resolution;

        while (this.frame <= stop.frame) {
            this.update();
            this.time = 0;
            for (let step = 0; step < resolution; step++) {
                this.time = timestep * step;
                // 4 game frames per second
                this.draw(4 * 60 / resolution);
                this.application.render();
                const canvas = this.application.renderer.extract.canvas();
                gif.addFrame(canvas, {
                    copy: true,
                    delay: 1000 / (4.0 * 10),
                });
            }
            this.frame++;
        }

        return new Promise((resolve) => {
            gif.on("finished", function(blob) {
                resolve(blob);
            });
            gif.render();
        });
    }

    get currentFrame() {
        return this.replay.full_frames[this.frame];
    }

    get currentStatistics() {
        let frame = this.currentFrame;
        let entities = {};
        let total_entities = 0;
        // TODO: update with Halite 3 statistics
        return {
            "planets": 0,
            "ships": 0,
            "total_ships": 0,
        };
    }

    attach(containerEl) {
        $(containerEl).append(this.application.view);

        this.keyboardControls.attach(document.body);

        this.update();
        this.application.render();

        this.application.ticker.add((dt) => {
            if (this.isPlaying()) {
                this.advanceTime(this.timeStep * this.playSpeed * dt);
            }
            this.keyboardControls.handleKeys(dt);
            this.draw(dt);

            if (!this.isPlaying() && this.animationQueue.length === 0) {
                console.log("Stopping event loop");
                this.application.stop();
            }
        });
    }

    play() {
        if (this._playing) return;
        this.application.render();
        this.application.start();

        this._playing = true;

        this.onPlay();
    }

    advanceTime(time) {
        // Interpolate between frames for smoother feel
        const prevFrame = this.frame;

        this.time += time;
        if (this.time >= 1.0) {
            this.frame++;
            this.time = 0;
        }
        else if (this.time < 0.0) {
            this.frame--;
            this.time = 1.0;
        }

        if (this.frame >= this.replay.full_frames.length) {
            this.pause();
            this.frame = this.replay.full_frames.length - 1;
            this.time = 1.0;
            this.onEnd();
            return;
        }
        else if (this.frame < 0) {
            this.pause();
            this.frame = 0;
            this.time = 0.0;
        }

        if (prevFrame !== this.frame) {
            this.update();
        }
        this.onUpdate();
    }

    pause() {
        if (!this._playing) return;
        this._playing = false;
        this.onPause();
    }

    scrub(frame, time, dt=1000/60) {
        this.pause();
        this.frame = frame;
        this.time = time;
        if (time === 0.0) {
            this.update();
            this.onUpdate();
        }
        this.draw(dt);
        // TODO: this is SLOW. Tie rerendering during scrub to rAF
        this.application.render();
    }

    update() {
        this.deathFlags = {};
        this.newEntities = [];

        if (this.currentFrame.events) {
            for (let event of this.currentFrame.events) {
                // How much to delay (in terms of ticks) before
                // actually playing the event
                const delayTime = 0;
                const cellSize = assets.CELL_SIZE * this.scale;

                if (event.type === "death") {
                    console.log("should be a death", event.owner_id, event.location);
                    // Use default draw function
                    this.animationQueue.push(
                        new animation.ShipExplosionFrameAnimation(
                            event, delayTime, cellSize, this.entityContainer));
                    if (typeof this.deathFlags[event.owner_id] === "undefined") {
                        this.deathFlags[event.owner_id] = {};
                    }
                    this.deathFlags[event.owner_id][event.location] = this.currentFrame;

                }
                else if (event.type === "spawn") {
                    // add the entity to the list of entities
                    let entity_object = {"x" : event.location[0], "y" : event.location[1], "energy" : event.energy, "owner": event.owner_id};
                    let new_entity = new playerSprite(this, entity_object);
                    this.entities.push(new_entity);
                    new_entity.attach(this.entityContainer);
                    this.animationQueue.push(new animation.FrameAnimation(
                        100, delayTime,
                        () => {
                        },
                        (frame) => {
                            const side = assets.CELL_SIZE * this.scale;

                            const entity_x = side * (event.location[0] + 0.5);
                            const entity_y = side * (event.location[1] + 0.5);
                            this.lights.lineStyle(2, assets.PLAYER_COLORS[event.owner_id], 0.5 * frame / 100);
                            this.lights.moveTo(entity_x - 0.5, entity_y - 0.5);
                            this.lights.lineTo(entity_x, entity_y);
                            this.lights.endFill();
                        },
                        () => {

                        }
                    ));
                }
                else {
                    console.log(event);
                }
            }
        }
        this.draw();
    }

    draw(dt=0) {
        this.lights.clear();

        // for (let factory of this.factories) {
        //     // TODO: check for living players and update factories with statuses
        // }

        // just let the fish wander
        // for (let fish of this.fish) {
        //     fish.update(this.time, dt);
        // }
        // TODO: determine map ownership
        //this.baseMap.update(this.currentFrame);

        for (let entity_index = 0; entity_index < this.entities.length; entity_index++) {
            let entity = this.entities[entity_index];
            let deathFrame = 500;
            if (this.deathFlags[entity.owner_id] &&
                typeof this.deathFlags[entity.owner_id][[entity.x, entity.y]] !== "undefined") {
                deathFrame = this.deathFlags[entity.owner_id][[entity.x, entity.y]];
                console.log("death frame");
            }

            if (this.frame < deathFrame) {
                entity.update();
            } else {
                entity.destroy();
                console.log("trying to delete entity");
                delete this.entities[entity_index];
            }
        }

        // dt comes from Pixi ticker, and the unit is essentially frames
        let queue = this.animationQueue;
        this.animationQueue = [];
        for (let anim of queue) {
            let subdelta = dt;
            if (anim.delayFrames > 0) {
                if (anim.delayFrames >= subdelta) {
                    anim.delayFrames -= subdelta;
                }
                else if (anim.delayFrames < subdelta) {
                    subdelta -= anim.delayFrames;
                    anim.delayFrames = -1;
                }
            }

            if (anim.delayFrames <= 0 && anim.frames >= subdelta) {
                anim.draw(anim.frames);
                anim.frames -= subdelta;
                this.animationQueue.push(anim);
            }
            else if (anim.delayFrames > 0) {
                this.animationQueue.push(anim);
            }
            else {
                anim.finish();
            }
        }
    }

    drawStats() {
        // TODO Add statistics for game play
    }

    render(dt=1000/60) {
        this.draw(dt);
        this.application.render();
    }

    isPlaying() {
        return this._playing;
    }
}
