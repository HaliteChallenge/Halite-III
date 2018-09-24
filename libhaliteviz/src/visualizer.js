import * as PIXI from "pixi.js";
import Ship from "./sprite";
import {Dropoff, Factory} from "./factory";
import {Map} from "./map";
import Camera from "./camera";
import * as statistics from "./statistics";
import * as keyboard from "./keyboardControls";

import * as assets from "./assets";
import colorTheme from "./colors";

import * as animation from "./animation";



class Signal {
    constructor(name) {
        this.name = name;
        this.listeners = [];
    }

    dispatch(...args) {
        this.listeners.forEach((f) => {
            try {
                f(...args);
            }
            catch (e) {
                console.error(`Error dispatching signal ${this.name}:`);
                console.error(e);
            }
        });
    }

    add(f) {
        this.listeners.push(f);
    }
}


export class HaliteVisualizer {
    constructor(replay, width, height) {
        window.replay = replay;
        window.visualizer = this;
        this.width = width || assets.BASE_VISUALIZER_WIDTH;
        this.height = height || assets.BASE_VISUALIZER_HEIGHT;

        this.replay = replay;
        this.map_width = replay.production_map.width;
        this.map_height = replay.production_map.height;
        this.stats = new statistics.Statistics(replay);

        this.frame = 0;
        this.prevFrame = -1;
        this.time = 0;
        this._playing = false;

        this.timeStep = 0.01;
        this.playSpeed = 5.0;
        this.scrubSpeed = 0.5;
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

        // XXX FORCE WEBGL ON SOFTWARE RENDERER
        // Copy of actual implementation, minus failIfMajorPerformanceCaveat
        PIXI.utils.isWebGLSupported = () => {
            const contextOptions = { stencil: true };
            try
            {
                if (!window.WebGLRenderingContext)
                {
                    return false;
                }
                const canvas = document.createElement('canvas');
                let gl = canvas.getContext('webgl', contextOptions) || canvas.getContext('experimental-webgl', contextOptions);
                const success = !!(gl && gl.getContextAttributes().stencil);
                if (gl)
                {
                    const loseContext = gl.getExtension('WEBGL_lose_context');
                    if (loseContext)
                    {
                        loseContext.loseContext();
                    }
                }
                gl = null;
                return success;
            }
            catch (e) {
                return false;
            }
        };
        this.application = new PIXI.Application(
            this.width, this.height,
            {
                backgroundColor: colorTheme()[0],
                antialias: true,
                resolution: 1,
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
            (kind, args) => this.onSelect.dispatch(kind, args), this.application.renderer);
        this.baseMap.attach(this.mapContainer);

        for (let i = 0; i < this.replay.players.length; i++) {
            const factoryBase = {"x" : this.replay.players[i].factory_location.x,
                "y" : this.replay.players[i].factory_location.y, "owner" : this.replay.players[i].player_id };
            const factory = new Factory(this, factoryBase, this.replay.constants,
                scale, (kind, args) => this.onSelect.dispatch(kind, args), this.application.renderer);
            this.factories.push(factory);
            factory.attach(this.factoryContainer);
        }

        this.container.filterArea = new PIXI.Rectangle(
            0, 0,
            this.application.renderer.width,
            this.application.renderer.height);
        this.container.addChild(this.mapContainer);
        this.container.addChild(this.factoryContainer);
        this.container.addChild(this.entityContainer);

        this.application.stage.addChild(this.container);
        this.application.stage.addChild(this.letterbox);

        this.timer = null;

        this.animationQueue = [];

        this.onUpdate = new Signal("onUpdate");
        this.onPlay = new Signal("onPlay");
        this.onPause = new Signal("onPause");
        this.onEnd = new Signal("onEnd");
        this.onSelect = new Signal("onSelect");
        this.onDeselect = new Signal("onDeselect");

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

    async encodeVideo(type="canvas") {
        console.log("Encoding video");
        const [ RecordRTC, html2canvas ] = await Promise.all([
            import(/* webpackChunkName: "recordrtc" */ 'recordrtc'),
            import(/* webpackChunkName: "html2canvas" */ 'html2canvas'),
        ]);
        window.html2canvas = html2canvas.default;

        this.pause();

        this.scrub(0, 0);
        const recorder = RecordRTC.default.bind({})(this.application.renderer.view, {
            type: type,
        });
        recorder.startRecording();
        this.play();
        const oldEnd = this.onEnd;

        return await new Promise((resolve) => {
            this.onEnd = new Signal("onEnd");
            this.onEnd.add(() => {
                this.onEnd = oldEnd;
                 recorder.stopRecording(() => {
                    const blob = recorder.getBlob();
                    console.log(blob);

                    resolve(blob);
                });
            });
        });
    }

    get currentFrame() {
        return this.replay.full_frames[this.frame];
    }

    /**
     * Helper method that figures out the current production present
     * on a cell, saving you from having to jump through the replay.
     */
    findCurrentProduction(frameId, x, y) {
        // TODO: this is inefficient AF
        // Start at previous frame because current frame's changed
        // cells record applies to the next frame. (Confused yet?)
        for (let i = frameId - 1; i >= 0; i--) {
            const frame = this.replay.full_frames[i];
            for (const cell of frame.cells) {
                if (cell.x == x && cell.y == y) {
                    return cell.production;
                }
            }
        }
        const cell = this.replay.production_map.grid[y][x];
        return cell.energy;
    }

    /**
     * Helper method that figures out the current status of the given
     * ship.
     */
    findShip(frameId, owner, id) {
        const frame = this.replay.full_frames[frameId];
        if (!frame) return null;

        if (frame.entities[owner][id]) {
            return frame.entities[owner][id];
        }
        // Not yet spawned, look for event
        for (const event of frame.events) {
            if (event.type === "spawn" &&
                event.owner_id === owner &&
                event.id === id) {
                return {
                    x: event.location.x,
                    y: event.location.y,
                    energy: event.energy,
                };
            }
        }
        return null;
    }

    attach(containerEl, keyboardEl=null) {
        containerEl = typeof containerEl === "string" ?
            document.querySelector(containerEl) : containerEl;
        containerEl.appendChild(this.application.view);
        if (!keyboardEl) {
            keyboardEl = document.body;
        }

        this.keyboardControls.attach(typeof keyboardEl === "string" ?
                                     document.querySelector(keyboardEl) :
                                     keyboardEl);

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

        this.onPlay.dispatch();
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
            this.onUpdate.dispatch();
            this.onEnd.dispatch();
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
        this.onUpdate.dispatch();
    }

    pause() {
        if (!this._playing) return;
        this._playing = false;
        this.onPause.dispatch();
    }

    scrub(frame, time, dt=1000/60) {
        const delta = frame > this.frame ? 1 : -1;
        this.pause();

        for (let f = this.frame + delta; (delta > 0 ? f < frame : f > frame); f += delta) {
            this.frame = f;
            this.time = 0.0;
            this.update(delta);
        }

        this.frame = frame;
        this.time = time;
        if (time === 0.0) {
            this.update(delta);
            this.onUpdate.dispatch();
        }
        this.draw(dt);
        // TODO: this is SLOW. Tie rerendering during scrub to rAF
        this.application.render();
    }

    // Update the status of the game once per frame. Updating includes processing turn movement, spawning, death, and map ownership
    // Be sure order of events (movement, merging, production calculation, spawning, and death) directly mirrors
    // game engine or visualization will be incorrect.
    update(delta=1) {
        for (const factory of this.factories) {
            factory.scale = this.camera.scale;
            factory.update(delta);
        }

        this.remove_invalid_entities();

        // Update all move_commands
        this.process_entity_commands();

        // Spawn entities (info from replay file), then process deaths
        this.process_entity_events(delta);

        // Process map ownership
        // The game writes the cells that changed during a turn to
        // that turn's frame, but we don't want to apply those changes
        // until the beginning of next turn.
        const cellsUpdatedLastTurn = this.replay.full_frames[this.frame - 1];
        this.baseMap.update(this.frame, cellsUpdatedLastTurn ? cellsUpdatedLastTurn.cells : [], delta);
    }

    /** Update/rerender after panning. */
    panRender() {
        this.draw();

        this.baseMap.update(this.frame, [], 0);

        if (!this.isPlaying()) {
            this.application.render();
        }
        this.onUpdate.dispatch();
    }

    /**
     * Remove entities that shouldn't be here anymore (due to
     * scrubbing).
     */
    remove_invalid_entities() {
        if (!this.currentFrame) return;

        for (const [entity_id, entity] of Object.entries(this.entity_dict)) {
            if (!this.currentFrame.entities[entity.owner] ||
                !this.currentFrame.entities[entity.owner][entity_id]) {
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
     * @param delta whether are are scrubbing forward or backward through time
     */
    process_entity_events(delta) {
        // TODO: add within frame interpolation
        let delayTime = 0;
        if (!this.currentFrame) return;
        if (this.currentFrame.events) {
            for (let event of this.currentFrame.events) {
                const cellSize = assets.CELL_SIZE;

                if (event.type === "shipwreck") {
                    const lastFrame = this.frame >= this.replay.full_frames.length - 1;
                    // add death animation to be drawn
                    // TODO: switch to Halite 3 animation
                    const involvedPlayers = event.ships.map((id) => {
                        for (const [player, playerShips] of
                             Object.entries(this.currentFrame.entities)) {
                            if (playerShips[id]) return player;
                        }
                        return null;
                    });
                    let color = 0xFFFFFF;
                    if (involvedPlayers.every(p => p === involvedPlayers[0])) {
                        color = assets.PLAYER_COLORS[parseInt(involvedPlayers[0], 10)];
                    }
                    this.animationQueue.push(
                        new animation.SpawnAnimation({
                            event,
                            frame: this.frame,
                            duration: 40,
                            cellSize,
                            container: this.container,
                        }));

                    // Don't actually remove entities - allow
                    // remove_invalid_entities to clean it up next
                    // frame (unless this is the last frame)
                    if (lastFrame) {
                        for (const id of event.ships) {
                            if (this.entity_dict[id]) {
                                this.entity_dict[id].destroy();
                                delete this.entity_dict[id];
                            }
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
                        // TODO: use new Halite 3 spawn animation
                        this.animationQueue.push(new animation.SpawnAnimation({
                            event,
                            frame: this.frame,
                            duration: 40,
                            cellSize,
                            container: this.container,
                        }));
                    }

                    // Store spawn as command so that entity knows not to mine this turn
                    if (!this.current_commands[event.owner_id]) {
                        this.current_commands[event.owner_id] = {};
                    }
                    this.current_commands[event.owner_id][event.id] = {"type" : "g"};
                }
                else if (event.type === "capture") {
                    this.animationQueue.push(new animation.SpawnAnimation({
                            event,
                            frame: this.frame,
                            duration: 40,
                            cellSize,
                            container: this.container,
                        }));
                }
                else if (event.type === "construct") {
                    /// TODO: create new sprite class for dropoffs, construct one, add to list (dict?) of dropoffs
                    // Temporarily draw as factory

                    if (delta < 0) {
                        // We are reversing, delete the factory

                        let idx = 0;
                        for (const dropoff of this.dropoffs) {
                            if (dropoff.factoryBase.x === event.location.x &&
                                dropoff.factoryBase.y === event.location.y) {
                                this.dropoffs.splice(idx, 1);
                                dropoff.destroy();
                                break;
                            }
                            idx++;
                        }

                        return;
                    }

                    this.animationQueue.push(new animation.SpawnAnimation({
                        event,
                        frame: this.frame,
                        duration: 5,
                        cellSize,
                        container: this.container,
                    }));

                    // Don't add factory twice (if scrubbing)
                    let create = true;
                    for (const dropoff of this.dropoffs) {
                        if (dropoff.factoryBase.x === event.location.x &&
                            dropoff.factoryBase.y === event.location.y) {
                            create = false;
                            break;
                        }
                    }

                    if (create) {
                        const dropoff_base = {"x" : event.location.x, "y" : event.location.y, "owner" : event.owner_id};
                        const dropoff = new Dropoff(this, dropoff_base, this.replay.constants,
                                                    this.camera.scale, (kind, args) => this.onSelect.dispatch(kind, args), this.application.renderer);
                        this.dropoffs.push(dropoff);
                        dropoff.attach(this.factoryContainer);
                    }

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

        this.baseMap.draw();

        // dt comes from Pixi ticker, and the unit is essentially frames
        let queue = this.animationQueue;
        this.animationQueue = [];
        let frameTime = this.frame + this.time;
        for (let anim of queue) {
            if (this.frame >= this.replay.full_frames.length - 1) {
                // If at end of replay, adjust animations to keep them playing
                anim.start -= this.timeStep * this.playSpeed * dt;
            }
            if (frameTime >= anim.start && frameTime <= anim.start + anim.duration) {
                anim.draw(this.camera, frameTime - anim.start);
                this.animationQueue.push(anim);
            }
            else if (frameTime < anim.start) {
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
