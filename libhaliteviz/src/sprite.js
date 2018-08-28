import * as PIXI from "pixi.js";
import {MotionBlurFilter} from '@pixi/filter-motion-blur';

import * as assets from "./assets";
import {CELL_SIZE, PLAYER_COLORS} from "./assets";

/**
 * Manages a ship on screen.
 */
export default class Ship {
    /**
     *
     * @param visualizer The visualizer object
     * @param record The sprite record. {x, y, owner, energy}
     */
    constructor(visualizer, record) {
        // Make a sprite a circle
        const spriteShape = new PIXI.Graphics();
        spriteShape.beginFill(assets.SPRITE_COLOR, 1);
        // draw circle - x coord, y coord, radius
        spriteShape.drawCircle(0, 0, assets.CELL_SIZE * 64);
        spriteShape.endFill();

        let spriteTexture = visualizer.application.renderer.generateTexture(spriteShape);

        this.sprite = new PIXI.Sprite(assets.TURTLE_SPRITES[record.owner]);
        this.highlight = new PIXI.Sprite(spriteTexture);
        this.highlight.visible = false;
        this.motionblur = new MotionBlurFilter([0, 0], 9, 0);
        this.sprite.filters = [this.motionblur];

        this.container = null;
        this.visualizer = visualizer;

        // Store map size to make movement easier
        this.map_width = this.visualizer.replay.production_map.width;
        this.map_height = this.visualizer.replay.production_map.height;

        this.owner = record.owner;
        this.energy = record.energy;
        this.id = record.id;
        this.x = record.x;
        this.y = record.y;

        // Set up sprite to be anchored in center and be square
        let setupSprite = (sprite, width) => {
            sprite.width = sprite.height = width;
            sprite.anchor.x = sprite.anchor.y = 0.5;
        };

        // Set up sprite size & anchors
        const width = assets.CELL_SIZE * this.visualizer.camera.scale;
        setupSprite(this.sprite, width * 4);
        setupSprite(this.highlight, width * 1.25);

        // add to board in correct position
        const pixelX = this.visualizer.camera.scale * CELL_SIZE * this.x + this.visualizer.camera.scale * CELL_SIZE / 2;
        const pixelY = this.visualizer.camera.scale * CELL_SIZE * this.y + this.visualizer.camera.scale * CELL_SIZE / 2;
        this.sprite.position.x = pixelX;
        this.sprite.position.y = pixelY;
    }

    /**
     * Add the sprite to the visualizer.
     * @param container {PIXI.Container} to use for the sprite
     */
    attach(container) {
        container.addChild(this.highlight);
        container.addChild(this.sprite);
        this.container = container;
    }

    /**
     * Remove this sprite from the visualizer(as in, when it runs out of health)
     */
    destroy() {
        this.container.removeChild(this.sprite);
        this.container.removeChild(this.highlight);
        delete this.container;
    }

    /**
     * TODO: update with selection of sprites
     */
    onClick() {
        this.visualizer.onSelect("ship", {
            owner: this.owner,
            id: this.id,
        });
    }

    /**
     * Update this sprite's display with the latest state from the replay.
     * @param command
     */
    update(command) {
        let direction = 0;
        let x_move = 0;
        let y_move = 0;
        // Move the sprite according to move commands and redraw in new location
        if (this.visualizer.frame < this.visualizer.replay.full_frames.length) {
            // Sprite spawned this turn, does not exist in entities struct at start of turn
            if (command.type === "g") {
                return;
            }
            const ownerEntities = this.visualizer.replay
                  .full_frames[this.visualizer.frame]
                  .entities[this.owner];
            if (!ownerEntities) return;
            const entity_record = ownerEntities[this.id];
            if (!entity_record) {
                return;
            }

            this.energy = entity_record.energy;

            if (command.type === "m") {
                if (command.direction === "n") {
                    direction = 0;
                    x_move = 0;
                    y_move = -1;
                }
                else if (command.direction === "e") {
                    direction = Math.PI / 2;
                    x_move = 1;
                    y_move = 0;
                }
                else if (command.direction === "s") {
                    direction = Math.PI;
                    x_move = 0;
                    y_move = 1;
                }
                else if (command.direction === "w") {
                    direction = -Math.PI / 2;
                    x_move = -1;
                    y_move = 0;
                }
                else {
                    // If still, preserve rotation
                    direction = this.sprite.rotation;
                }
                this.sprite.rotation = direction;

                // To prevent "glitching" when a move is recorded that
                // isn't processed (because there wasn't enough
                // energy, for instance), we interpolate with the next
                // frame's position where available.
                if (this.visualizer.frame < this.visualizer.replay.full_frames.length - 1) {
                    const next_frame = this.visualizer.replay
                          .full_frames[this.visualizer.frame + 1];
                    if (next_frame.entities[this.owner] &&
                        next_frame.entities[this.owner][this.id]) {
                        const next_record = next_frame.entities[this.owner][this.id];
                        x_move = next_record.x - entity_record.x;
                        y_move = next_record.y - entity_record.y;

                        // Wraparound
                        if (x_move > 1) {
                            x_move = -1;
                        }
                        else if (x_move < -1) {
                            x_move = 1;
                        }
                        if (y_move > 1) {
                            y_move = -1;
                        }
                        else if (y_move < -1) {
                            y_move = 1;
                        }
                    }
                }
            }  else if (command.type === "d") {
                // TODO
            } else if (command.type === "m") {
                // TODO
            } else if (command.type === "c") {
                // TODO
            }

            // Use wrap around map in determining movement,
            // interpolate between moves with visualizer time
            // Use a bit of easing on the time to make it look
            // nicer (cubic in/out easing)
            let t = this.visualizer.time;
            this.motionblur.velocity.set(x_move * -20 * t * (t - 1),
                                         y_move * -20 * t * (t - 1));
            t /= 0.5;
            if (t < 1) {
                t = t*t*t/2;
            }
            else {
                t -= 2;
                t = (t*t*t + 2)/2;
            }

            this.x = (entity_record.x + x_move * t + this.map_width) % this.map_width;
            this.y = (entity_record.y + y_move * t + this.map_height) % this.map_height;
        }
    }

    draw() {
        // Determine pixel location from grid location, then move sprite
        const size = this.visualizer.camera.scale * CELL_SIZE;
        // Account for camera panning
        const [ cellX, cellY ] = this.visualizer.camera.worldToCamera(this.x, this.y);
        const pixelX = size * cellX + this.visualizer.camera.scale * CELL_SIZE / 2;
        const pixelY = size * cellY + this.visualizer.camera.scale * CELL_SIZE / 2;
        this.sprite.position.x = pixelX;
        this.sprite.position.y = pixelY;
        this.sprite.width = this.sprite.height = size * 1.5;
        this.highlight.position.x = pixelX;
        this.highlight.position.y = pixelY;
        this.highlight.width = this.highlight.height = 1.25 * size;

        const camera = this.visualizer.camera;
        this.highlight.visible =
            camera.selected &&
            camera.selected.type === "ship" &&
            camera.selected.id === this.id &&
            camera.selected.owner === this.owner;

        if (!this.visualizer.currentFrame || !this.visualizer.currentFrame.entities) {
            return;
        }
        if (!this.visualizer.currentFrame.entities[this.owner]) {
            return;
        }
        if (!this.visualizer.currentFrame.entities[this.owner][this.id]) {
            return;
        }

        this.highlight.visible =
            this.highlight.visible ||
            this.visualizer.currentFrame.entities[this.owner][this.id].is_inspired;
    }
}
