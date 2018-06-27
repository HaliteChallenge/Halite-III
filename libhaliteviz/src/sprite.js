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
        spriteShape.beginFill(assets.SPRITE_COLOR, assets.SPRITE_ALPHA);
        // draw circle - x coord, y coord, radius
        spriteShape.drawCircle(0, 0, assets.CELL_SIZE * 10);
        spriteShape.endFill();

        let spriteTexture = visualizer.application.renderer.generateTexture(spriteShape);

        this.sprite = new PIXI.Sprite(spriteTexture);
        // Halo's seem to be used as visual display of health, so keeping for now. 
        this.halo = PIXI.Sprite.from(assets.HALO_IMAGE);

        this.container = null;
        this.visualizer = visualizer;

        // Store map size to make movement easier
        this.map_width = this.visualizer.replay.production_map.width;
        this.map_height = this.visualizer.replay.production_map.height;

        this.owner = record.owner;
        this.energy = record.energy;
        this.x = record.x;
        this.y = record.y;

        let setupSprite = (sprite, radius) => {
            sprite.width = sprite.height = 2 * radius * this.visualizer.scale;
            sprite.anchor.x = sprite.anchor.y = 0.5;
        };

        // TODO: convert to more appropriate variable name
        const radius = this.visualizer.CELL_SIZE;
        this.sprite.anchor.set(0.5);
        this.sprite.width = 0.7 * radius * this.visualizer.scale;
        this.sprite.height = 0.7 * radius * this.visualizer.scale;
        setupSprite(this.halo, radius);

        this.sprite.tint = PLAYER_COLORS[this.owner];
        this.halo.tint = PLAYER_COLORS[this.owner];

        this.halo.interactive = true;
        this.halo.buttonMode = true;
        this.halo.on("pointerdown", this.onClick.bind(this));


        this.update(record);

    }

     /**
     * Add the sprite to the visualizer.
     * @param container {PIXI.Container} to use for the sprite
     */
    attach(container) {
        container.addChild(this.halo, this.sprite);
        this.container = container;
    }

    /**
     * Remove this sprite from the visualizer(as in, when it runs out of health)
     */
    destroy() {
        this.container.removeChild(this.halo);
        this.container.removeChild(this.sprite);
    }


    /**
     * TODO: Add comment saying what this thing does
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

    update(record) {
        const max_sprite_health = this.visualizer.replay.GAME_CONSTANTS.MAX_ENERGY;
        const health_factor = 0.1 + 0.2 * record.energy / max_sprite_health;

        this.halo.alpha = health_factor;

        let direction = 0;
        let x_move = 0;
        let y_move = 0;

        // Draw the exhaust trail based on the thrust command issued this turn
        //this.exhaust.visible = false;
        if (this.visualizer.frame < this.visualizer.replay.frames.length - 1) {
            let moves = this.visualizer.replay.full_frames[this.visualizer.frame].moves;
            let move = moves[record.owner];
            if (move && move.type === "move") {
                if (move.direction === "n") {
                    direction = Math.PI;
                    x_move = 0;
                    y_move = 1;

                }
                if (move.direction === "e") {
                    direction = Math.PI / 2;
                    x_move = 1;
                    y_move = 0;

                }
                if (move.direction === "s") {
                    direction = 0;
                    x_move = 0;
                    y_move = -1;

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
            }
        }

        const pixelX = this.visualizer.scale * CELL_SIZE * this.x;
        const pixelY = this.visualizer.scale * CELL_SIZE * this.y;
        this.halo.position.x = this.sprite.position.x = pixelX;
        this.halo.position.y = this.sprite.position.y = pixelY;
    }
}

