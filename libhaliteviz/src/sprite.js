import * as PIXI from "pixi.js";

import * as assets from "./assets";
import {CELL_SIZE, PLAYER_COLORS} from "./assets";

/**
 * Manages a Sprite (Boat?) on screen.
 */
 export class playerSprite {
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
        spriteShape.drawCircle(0, 0, assets.CELL_SIZE * visualizer.scale);
        spriteShape.endFill();

        let spriteTexture = visualizer.application.renderer.generateTexture(spriteShape);

        this.sprite = new PIXI.Sprite(spriteTexture);
        this.sprite.zOrder= -1;

        this.container = null;
        this.visualizer = visualizer;

        // Store map size to make movement easier
        this.map_width = this.visualizer.replay.production_map.width;
        this.map_height = this.visualizer.replay.production_map.height;
        this.energy_loss = this.visualizer.replay.GAME_CONSTANTS.BASE_TURN_ENERGY_LOSS;

        this.owner = record.owner;
        this.energy = record.energy;
        this.x = record.x;
        this.y = record.y;

        let setupSprite = (sprite, width) => {
            sprite.width = sprite.height = width
            sprite.anchor.x = sprite.anchor.y = 0.5;
        };

        // Set up sprite size & anchors
        const width = assets.CELL_SIZE * this.visualizer.scale;
        setupSprite(this.sprite, width);

        this.sprite.tint = PLAYER_COLORS[this.owner];

        // add to board in correct position
        const pixelX = this.visualizer.scale * CELL_SIZE * this.x + this.visualizer.scale * CELL_SIZE / 2;
        const pixelY = this.visualizer.scale * CELL_SIZE * this.y + this.visualizer.scale * CELL_SIZE / 2;
        this.sprite.position.x = pixelX;
        this.sprite.position.y = pixelY;
    }

     /**
     * Add the sprite to the visualizer.
     * @param container {PIXI.Container} to use for the sprite
     */
    attach(container) {
        container.addChild(this.sprite);
        this.container = container;
    }

    /**
     * Remove this sprite from the visualizer(as in, when it runs out of health)
     */
    destroy() {
        this.container.removeChild(this.sprite);
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
     * @param record
     */

    update() {
        let direction = 0;
        let x_move = 0;
        let y_move = 0;

        // Move the sprite according to move commands and redraw in new location
        if (this.visualizer.frame < this.visualizer.replay.full_frames.length - 1) {
            let moves = this.visualizer.replay.full_frames[this.visualizer.frame].moves;
            let player_moves = moves[this.owner];
            for (let move_idx = 0; move_idx < player_moves.length; move_idx++) {
                let move = player_moves[move_idx];
                if (move && move.type === "move" && move.entity_x === this.x && move.entity_y === this.y) {
                    if (move.direction === "n") {
                        direction = Math.PI;
                        x_move = 0;
                        y_move = -1;

                    }
                    if (move.direction === "e") {
                        direction = Math.PI / 2;
                        x_move = 1;
                        y_move = 0;

                    }
                    if (move.direction === "s") {
                        direction = 0;
                        x_move = 0;
                        y_move = 1;

                    }
                    if (move.direction === "w") {
                        direction = -Math.PI / 2;
                        x_move = -1;
                        y_move = 0;

                    }
                    this.sprite.rotation = direction;

                    // Use wrap around map in determining movement
                    this.x = (this.x + x_move + this.map_width) % this.map_width;
                    this.y = (this.y + y_move + this.map_height) % this.map_height;
                    break;
                }
            }
        }
        this.energy -= this.energy_loss;

        const pixelX = this.visualizer.scale * CELL_SIZE * this.x + this.visualizer.scale * CELL_SIZE / 2;
        const pixelY = this.visualizer.scale * CELL_SIZE * this.y + this.visualizer.scale * CELL_SIZE / 2;
        this.sprite.position.x = pixelX;
        this.sprite.position.y = pixelY;
    }
}

