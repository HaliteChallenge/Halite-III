import * as PIXI from "pixi.js";

import * as assets from "./assets";
import {CELL_SIZE, PLAYER_COLORS} from "./assets";

/**
 * Manages a Sprite (Boat?) on screen.
 */
 export class playerSprite {
    /**
     *
     * @param visualizer The visualizer object - what's a visualizer object/where is it defined??
     * @param record The sprite record from the replay - is this the json file? who knows....
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

        this.owner = record.owner;
        this.id = record.id;

        let setupSprite = (sprite, radius) => {
            sprite.width = sprite.height = 2 * radius * this.visualizer.scale;
            sprite.anchor.x = sprite.anchor.y = 0.5;
        };

           // What's the weapon radius/ how is it visualized?
           // TODO: use something different for purpose of WEAPON RADIUS visualization
        const radius = this.visualizer.replay.constants.WEAPON_RADIUS;
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
        const max_sprite_health = this.visualizer.replay.constants.MAX_SHIP_HEALTH;
        const health_factor = 0.1 + 0.2 * record.health / max_sprite_health;

        this.halo.alpha = health_factor;

        let direction = 0;
        let vel_x = 0;
        let vel_y = 0;
        // Draw the exhaust trail based on the thrust command issued this turn
        //this.exhaust.visible = false;
        if (this.visualizer.frame < this.visualizer.replay.frames.length - 1) {
            let moves = this.visualizer.replay.moves[this.visualizer.frame];
            let move = moves[record.owner][0][record.id];
            if (move && move.type === "thrust" && record.docking.status === "undocked") {
                // Adds the command to the ship velocity
                let angle = move.angle * Math.PI / 180;
                vel_x += move.magnitude * Math.cos(angle);
                vel_y += move.magnitude * Math.sin(angle);

                const vel_factor = move.magnitude / this.visualizer.replay.constants.MAX_ACCELERATION;

                // this.exhaust.visible = true;
                // this.exhaust.rotation = angle + Math.PI / 2;
                // this.exhaust.alpha = 0.4 * vel_factor;
                // this.exhaust.height = 0.6 * vel_factor * (120 / 11) * this.exhaust.width;
            }
        }

        // if (this.visualizer.frame < this.visualizer.replay.frames.length - 1) {
        //     let moves = this.visualizer.replay.moves[this.visualizer.frame];
        //     let move = moves[record.owner][0][record.id];
        //     if (move && move.type === "move" ) {
        //         // Adds the command to the ship velocity
        //         let direction = move.direction;
        //     }
            // TODO: add rotation code - just four ifs? seems messy
            // this.sprite.rotation = 
            // Adjust our position based on the time (interpolate between frames)
            // if (move.direction === "N") {
            //     direction =  Math.PI;
            //     const x_move = 0;
            //     const y_move = 1;

            // }
            // if (move.direction === "E") {
            //     direction =  Math.PI / 2;
            //     const x_move = 1;
            //     const y_move = 0;

            // }
            // if (move.direction === "S") {
            //     direction =  0;
            //     const x_move = 0;
            //     const y_move = -1;

            // }
            // if (move.direction === "W") {
            //     direction = - Math.PI / 2;
            //     const x_move = -1;
            //     const y_move = 0;

            // }
            // this.sprite.rotation = direction;

             // Orient the sprite in the direction that it's moving
        let angle = Math.atan2(vel_y, vel_x);
        this.sprite.rotation = angle + Math.PI / 2;

        // Adjust speed to not go over max speed - this also doesn't matter
        // without drag/inertia (it's to make sure our forecasted positions
        // line up with what the game environment did)
        const max_speed = this.visualizer.replay.constants.MAX_SPEED;
        const magnitude = Math.sqrt(vel_x*vel_x + vel_y*vel_y);
        if (magnitude > max_speed) {
            vel_x *= magnitude / max_speed;
            vel_y *= magnitude / max_speed;
        }

        // Adjust our position based on the time (interpolate between frames)
        const x = record.x + this.visualizer.time * vel_x;
        const y = record.y + this.visualizer.time * vel_y;

        // const x = record.x + x_move;
        // const y = record.y + y_move;

        const pixelX = this.visualizer.scale * CELL_SIZE * x;
        const pixelY = this.visualizer.scale * CELL_SIZE * y;
        this.halo.position.x = this.sprite.position.x = pixelX;
        this.halo.position.y = this.sprite.position.y = pixelY;
        
    }
}

