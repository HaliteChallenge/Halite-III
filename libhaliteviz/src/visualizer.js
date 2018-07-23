const PIXI = require("pixi.js");
const $ = require("jquery");
const extraFilters = require("pixi-extra-filters");

import Ship from "./sprite";
import {Factory} from "./factory";
import {Map} from "./map";
import Camera from "./camera";
import * as statistics from "./statistics";
import * as keyboard from "./keyboardControls";

import * as assets from "./assets";

import * as animation from "./animation";


export class HaliteVisualizer {
    constructor(replay, width, height) {
        window.replay = replay;
        this.width = width || assets.BASE_VISUALIZER_WIDTH;
        this.height = height || assets.BASE_VISUALIZER_HEIGHT;

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
            this.width, this.height,
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

        // Set initial scale so things fit exactly in the visible area
        let scale = this.height / (this.map_height * assets.CELL_SIZE);
        if (this.map_width * scale * assets.CELL_SIZE > this.width) {
            scale = this.width / (this.map_width * assets.CELL_SIZE);
        }

        this.container = new PIXI.Container();

        this.container.position.x = 0;
        this.container.position.y = 0;
        this.letterbox = new PIXI.Graphics();
        if (this.container.position.y > 0) {
            this.letterbox.beginFill(0xFFFFFF);
            this.letterbox.drawRect(0, 0, this.width, this.container.position.y);
            this.letterbox.drawRect(
                0,
                this.container.position.y + this.map_height * scale * assets.CELL_SIZE,
                this.width,
                this.container.position.y);
        }
        if (this.container.position.x > 0) {
            this.letterbox.beginFill(0xFFFFFF);
            this.letterbox.drawRect(0, 0, this.container.position.x, this.height);
            this.letterbox.drawRect(
                this.container.position.x + this.map_width * scale * assets.CELL_SIZE,
                0,
                this.container.position.x,
                this.height);
        }


        this.mapContainer = new PIXI.Container();
        this.factoryContainer = new PIXI.Container();
        this.entityContainer = new PIXI.Container();

        this.entity_dict = {};
        this.current_commands = {};
        this.factories = [];
        this.dropoffs = [];

        this.camera = new Camera(this, scale);

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
        }

        this.container.addChild(this.mapContainer);
        this.container.addChild(this.factoryContainer);
        this.container.addChild(this.entityContainer);

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

    resize(width, height) {
        this.application.renderer.resize(width, height);
        this.width = width;
        this.height = height;
        // TODO: redo initial scale, letterboxing
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
        for (const sprite of Object.values(this.entities_dict)) {
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

        if (this.frame > this.replay.full_frames.length) {
            this.pause();
            this.frame = this.replay.full_frames.length - 1;
            this.time = 1.0;
            this.onUpdate();
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
        const delta = frame > this.frame ? 1 : -1;
        this.pause();

        for (let f = this.frame + delta; (delta > 0 ? f < frame : f > frame); f += delta) {
            this.frame = f;
            this.time = 0.0;
            this.update();
        }

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

        this.remove_invalid_entities();

        // Update all move_commands
        this.process_entity_commands();

        // Spawn entities (info from replay file), then process deaths
        this.process_entity_events();

        // Process map ownership
        if (this.currentFrame) {
            this.baseMap.update(this.currentFrame.cells);
        }
    }

    /** Update/rerender after panning. */
    panRender() {
        this.draw();
        // for (const factory of this.factories) {
        //     factory.update();
        // }

        // for (const dropoff of this.dropoffs) {
        //     dropoff.update();
        // }

        this.baseMap.update([]);

        if (!this.isPlaying()) {
            this.application.render();
        }
        this.onUpdate();
    }

    /**
     * Remove entities that shouldn't be here anymore (due to
     * scrubbing).
     */
    remove_invalid_entities() {
        if (!this.currentFrame) return;

        for (const [entity_id, entity] of Object.entries(this.entity_dict)) {
            if (!this.currentFrame.entities[entity.owner][entity_id]) {
                const sprite = this.entity_dict[entity_id];
                sprite.destroy();
                delete this.entity_dict[entity_id];
            }
        }

        // Spawn entities that don't yet exist when we're panning
        for (const [ownerId, ships] of Object.entries(this.currentFrame.entities)) {
            for (const [entityId, ship] of Object.entries(ships)) {
                if (!this.entity_dict[entityId]) {
                    const record = {
                        x: ship.x,
                        y: ship.y,
                        energy: ship.energy,
                        owner: ownerId,
                        id: entityId,
                    };
                    this.entity_dict[entityId] = new Ship(this, record);
                    this.entity_dict[entityId].attach(this.entityContainer);
                }
            }
        }

        // TODO: do the same for dropoffs
    }

    /**
     * Moves all entities for a turn
     * First removes all entities from map, then has each entity independently calculate its new location from this turn's
     * commands, the re-adds entities to map, merging in the process. This order ensures that merges happen only after all
     * of a player's move commands have been processed.
     */
    process_entity_commands() {
        this.current_commands = {};
        if (!this.currentFrame) return;
        for (let player_id in this.currentFrame.moves) {
            // TODO check desired and actual type of player_id
            this.current_commands[player_id] = {};
            for (let command_key in this.currentFrame.moves[player_id]) {
                let command = this.currentFrame.moves[player_id][command_key];
                const command_type = command.type;
                if (command_type === "m" || command_type === "d" || command_type === "c") {
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
        if (!this.currentFrame) return;
        if (this.currentFrame.events) {
            for (let event of this.currentFrame.events) {
                const cellSize = assets.CELL_SIZE;

                if (event.type === "shipwreck") {
                    // add death animation to be drawn
                    // TODO: switch to Halite 3 animation
                    this.animationQueue.push(
                        new animation.ShipExplosionFrameAnimation(
                            event, delayTime, cellSize, this.entityContainer));
                    // Remove all entities involved in crash
                    for (let index = 0; index < event.ships.length; index++) {
                        const entity_id = event.ships[index];
                        // Might not actually exist when we're panning
                        if (this.entity_dict[entity_id]) {
                            this.entity_dict[entity_id].destroy();
                            delete this.entity_dict[entity_id];
                        }
                    }

                }
                else if (event.type === "spawn") {
                    // Create a new entity, add to map, and merge as needed
                    const entity_object = {
                        x: event.location.x,
                        y: event.location.y,
                        energy: event.energy,
                        owner: event.owner_id,
                        id: event.id,
                    };

                    if (!this.entity_dict[event.id]) {
                        const new_entity = new Ship(this, entity_object);
                        this.entity_dict[event.id] = new_entity;
                        new_entity.attach(this.entityContainer);
                    }

                    // TODO: use new Halite 3 spawn animation
                    this.animationQueue.push(
                        new animation.PlanetExplosionFrameAnimation(
                            event, delayTime, cellSize, this.entityContainer));

                    // Store spawn as command so that entity knows not to mine this turn
                    if (!this.current_commands[event.owner_id]) {
                        this.current_commands[event.owner_id] = {};
                    }
                    this.current_commands[event.owner_id][event.id] = {"type" : "g"};
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
                        this.camera.scale, (kind, args) => this.onSelect(kind, args), this.application.renderer);
                    this.dropoffs.push(dropoff);
                    dropoff.attach(this.factoryContainer);

                    // delete entity sprite as it is no longer a ship
                    // entity might not exist if this is the zero
                    // frame (which just records prior game state,
                    // e.g. from a snapshot)
                    if (this.entity_dict[event.id]) {
                        this.entity_dict[event.id].destroy();
                        delete this.entity_dict[event.id];
                    }
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
        for (const factory of this.factories) {
            factory.draw();
        }

        for (const dropoff of this.dropoffs) {
            dropoff.draw();
        }

        for (const entity_id in this.entity_dict) {
            const entity = this.entity_dict[entity_id];
            if (this.current_commands.hasOwnProperty(entity.owner)
                && this.current_commands[entity.owner].hasOwnProperty(entity_id)) {
                entity.update(this.current_commands[entity.owner][entity_id]);
            } else {
                // no command implies entity is mining
                entity.update({"type" : "m"});
            }

            entity.draw();
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
