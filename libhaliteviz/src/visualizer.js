const PIXI = require("pixi.js");
const $ = require("jquery");
const extraFilters = require("pixi-extra-filters");

import {playerSprite} from "./sprite";
import {Factory} from "./factory";
import {Fish} from "./fish";
import {Map} from "./map";
import Camera from "./camera";
import * as statistics from "./statistics";
import * as keyboard from "./keyboardControls";

import * as assets from "./assets";

import * as animation from "./animation";


export class HaliteVisualizer {
    constructor(replay) {
        this.replay = replay;
        this.map_width = replay.production_map.width;
        this.map_height = replay.production_map.height;
        //this.stats = new statistics.Statistics(replay);

        this.frame = 0;
        this.prevFrame = -1;
        this.time = 0;
        this._playing = false;

        this.timeStep = 0.01;
        this.playSpeed = 2.0;
        this.scrubSpeed = 0.25;
        // Keyboard controls - map a key name to an action, or directly to
        // a handler
        this.keyboardControls = new keyboard.KeyboardControls(this, {
            "ArrowLeft": "scrubBackwards",
            "ArrowRight": "scrubForwards",
            "KeyW": "panUp",
            "KeyS": "panDown",
            "KeyA": "panLeft",
            "KeyD": "panRight",
            "Equal": "zoomIn",
            "Minus": "zoomOut",
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
        let scale = assets.VISUALIZER_HEIGHT / (this.map_height * assets.CELL_SIZE);
        if (this.map_width * scale * assets.CELL_SIZE > assets.VISUALIZER_SIZE) {
            scale = assets.VISUALIZER_SIZE / (this.map_width * assets.CELL_SIZE);
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
                this.container.position.y + this.map_height * scale * assets.CELL_SIZE,
                assets.VISUALIZER_SIZE,
                this.container.position.y);
        }
        if (this.container.position.x > 0) {
            this.letterbox.beginFill(0xFFFFFF);
            this.letterbox.drawRect(0, 0, this.container.position.x, assets.VISUALIZER_HEIGHT);
            this.letterbox.drawRect(
                this.container.position.x + this.map_width * scale * assets.CELL_SIZE,
                0,
                this.container.position.x,
                assets.VISUALIZER_HEIGHT);
        }


        this.mapContainer = new PIXI.Container();
        this.factoryContainer = new PIXI.Container();
        this.entityContainer = new PIXI.Container();
        this.fishContainer = new PIXI.Container();

        // Store entities both in list for easy iteration and in 2d array to understand spatial relationships
        this.entities = Array.from(Array(this.map_height), () => new Array(this.map_width));
        this.entities_list = [];
        this.entity_dict = {};
        this.current_commands = {};
        this.factories = [];
        this.dropoffs = [];
        this.fish = [];

        this.camera = new Camera(
            scale, this.panRender.bind(this),
            this.replay.production_map.width,
            this.replay.production_map.height
        );

        // Generate base map with visualziation of production squares
        this.baseMap = new Map(this.replay, this.replay.GAME_CONSTANTS, this.camera,
            (kind, args) => this.onSelect(kind, args), this.application.renderer);
        this.baseMap.attach(this.mapContainer);

        for (let i = 0; i < this.replay.players.length; i++) {
            const factoryBase = {"x" : this.replay.players[i].factory_location.x,
                "y" : this.replay.players[i].factory_location.y, "owner" : this.replay.players[i].player_id };
            const factory = new Factory(this, factoryBase, this.replay.constants,
                scale, (kind, args) => this.onSelect(kind, args), this.application.renderer);
            this.factories.push(factory);
            factory.attach(this.factoryContainer);

            // Add players'initial entities to list and map
            for (let entity_json of this.replay.players[i].entities) {
                let entity_object = {"x" : entity_json.x, "y" : entity_json.y, "energy" : entity_json.energy, "owner": this.replay.players[i].player_id};
                let new_entity = new playerSprite(this, entity_object);
                if (typeof this.entities[new_entity.y][new_entity.x] === "undefined") {
                    this.entities[new_entity.y][new_entity.x] = {};
                }
                this.entities[new_entity.y][new_entity.x][new_entity.owner] = new_entity;
                this.entities_list.push(new_entity);
                new_entity.attach(this.entityContainer);
            }

            // TODO: Re-add fish with herding logic
            // const fish = new Fish(this.replay.constants, scale, (kind, args) => this.onSelect(kind, args));
            // this.fish.push(fish);
            // fish.attach(this.fishContainer);
        }

        // Prerender the points of interest once, and keep it as a texture

        this.container.addChild(this.mapContainer);
        this.container.addChild(this.factoryContainer);
        this.container.addChild(this.entityContainer);
        this.container.addChild(this.fishContainer);

        this.application.stage.addChild(this.container);
        this.application.stage.addChild(this.letterbox);

        this.timer = null;

        this.animationQueue = [];

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
     * Generate a string used to load the game from a certain point.
     */
    snapshot(frame) {
        const parts = [];
        parts.push(this.replay.ENGINE_VERSION);
        parts.push([
            this.replay.production_map.map_generator,
            this.replay.production_map.width,
            this.replay.production_map.height,
            this.replay.number_of_players,
            this.replay.map_generator_seed,
        ].join(","));

        const spritesByOwner = [];
        for (const sprite of this.entities_list) {
            if (!sprite) continue;
            if (!spritesByOwner[sprite.owner]) {
                spritesByOwner[sprite.owner] = [];
            }
            const { x, y, energy } = sprite;
            spritesByOwner[sprite.owner].push(`${x}-${y}-${energy}`);
        }
        spritesByOwner.forEach((ownedSprites, ownerId) => {
            parts.push(ownerId);
            parts.push(this.replay.full_frames[this.frame].energy[ownerId]);
            const factories = [];
            for (const factory of this.factories) {
                if (factory.owner === ownerId) {
                    const { x, y } = factory.factoryBase;
                    factories.push(`${x}-${y}`);
                }
            }
            parts.push(factories.join(","));

            parts.push(ownedSprites.join(","));
        });

        return parts.join(";");
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
        // TODO: destroy entities, factories
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
        // TODO: update with Halite 3 statistics (keep stubs of old statistics as they are expected by the rest of the website
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

        // Handle panning and zooming
        this.camera.attach(this.application.view);
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
        this.prevFrame = prevFrame;
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

    // Update the status of the game once per frame. Updating includes processing turn movement, spawning, death, and map ownership
    // Be sure order of events (movement, merging, production calculation, spawning, and death) directly mirrors
    // game engine or visualization will be incorrect.
    update() {
        for (const factory of this.factories) {
            factory.scale = this.camera.scale;
            factory.update();
        }

        // Update all move_commands
        this.process_entity_commands();

        // Process map ownership
        this.baseMap.update(this.replay.full_frames[this.frame].cells);

        // Spawn entities (info from replay file), then process deaths
        this.process_entity_events();
        //this.process_entity_energy_loss();
    }

    /** Update/rerender after panning. */
    panRender() {
        for (const sprite of Object.values(this.entities_dict)) {
            if (sprite) sprite.updatePosition();
        }

        for (const factory of this.factories) {
            factory.update();
        }

        for (const dropoff of this.dropoffs) {
            dropoff.update();
        }

        this.baseMap.update([]);

        if (!this.isPlaying()) {
            this.application.render();
        }
        this.onUpdate();
    }

    /**
     * Moves all entities for a turn
     * First removes all entities from map, then has each entity independently calculate its new location from this turn's
     * commands, the re-adds entities to map, merging in the process. This order ensures that merges happen only after all
     * of a player's move commands have been processed.
     */
    process_entity_commands() {
        this.current_commands = {};
        for (let player_id in Object.keys(this.replay.full_frames[this.frame].moves)) {
            // TODO check desired and actual type of player_id
            this.current_commands[player_id] = {};
            for (let command in this.replay.full_frames[this.frame].moves[player_id]) {
                const command_type = command.type;
                if (command_type === "move" || command_type === "dump" || command_type === "construct") {
                    this.current_commands[player_id][command.id] = command;
                }
            }
        }
    }

    /**
     * Read events for the turn from the replay file.
     * This functions will spawn new entities, and create animations for spawning and deaths.
     * Though this function adds animations for deaths, it does not remove dying entities.
     */
    process_entity_events() {
        // TODO: add within frame interpolation
        let delayTime = 0;
        if (this.replay.full_frames[this.frame].events) {
            for (let event of this.replay.full_frames[this.frame].events) {
                const cellSize = assets.CELL_SIZE;

                if (event.type === "shipwreck") {
                    // add death animation to be drawn
                    // TODO: switch to Halite 3 animation
                    this.animationQueue.push(
                        new animation.ShipExplosionFrameAnimation(
                            event, delayTime, cellSize, this.entityContainer));
                    // Remove all entities involved in crash
                    for (let entity_id in ships) {
                        this.entity_dict[entity_id].destroy();
                        delete this.entity_dict[entity_id];
                    }

                }
                else if (event.type === "spawn") {
                    // Create a new entity, add to map, and merge as needed
                    let entity_object = {"x" : event.location.x, "y" : event.location.y,
                                        "energy" : event.energy, "owner": event.owner_id, "id" : event.id};
                    let new_entity = new playerSprite(this, entity_object);

                    this.entity_dict[event.id] = new_entity;
                    new_entity.attach(this.entityContainer);

                    // TODO: use new Halite 3 spawn animation
                    this.animationQueue.push(
                        new animation.PlanetExplosionFrameAnimation(
                            event, delayTime, cellSize, this.entityContainer));
                    // Store spawn as command so that entity knows not to mine this turn
                    this.current_commands[event.owner_id][event.id] = {"type" : "spawn"};
                }
                else if (event.type === "construct") {
                    /// TODO: create new sprite class for dropoffs, construct one, add to list (dict?) of dropoffs
                    // temporarily use old animation
                    this.animationQueue.push(
                        new animation.PlanetExplosionFrameAnimation(
                            event, delayTime, cellSize, this.factoryContainer));
                    // Temporarily draw as factory
                    const dropoff_base = {"x" : event.location.x, "y" : event.location.y, "owner" : event.owner_id};
                    const dropoff = new Factory(this, dropoff_base, this.replay.constants,
                        scale, (kind, args) => this.onSelect(kind, args), this.application.renderer);
                    this.dropoffs.push(dropoff);
                    dropoff.attach(this.factoryContainer);

                    // delete entity sprite as it is no longer a ship
                    this.entities_dict[event.id].destroy();
                    delete this.entities_dict[event.id];
                }
                else {
                    console.log(event);
                }
            }
        }
    }

    /**
     * Draw animations. Draw called more than once per frame
     * @param dt
     */
    draw(dt=0) {
        // TODO: update fish with herding mechanism
        // just let the fish wander
        // for (let fish of this.fish) {
        //     fish.update(this.time, dt);
        // }
        for (let entity_id in Object.keys(this.entity_dict)) {
            let entity = this.entity_dict[entity];
            if (this.current_commands[entity.owner_id].hasOwnProperty(entity_id)) {
                entity.update(this.current_commands[entity.owner_id][entity_id]);
            } else {
                // no command implies entity is mining
                entity.update({"type" : "mine"})
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
                anim.draw(this.camera, anim.frames);
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
