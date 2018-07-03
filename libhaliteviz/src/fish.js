import * as PIXI from "pixi.js";

import * as assets from "./assets";

/**
 * Manages a fish on screen.
 */
export class Fish {
    /**
     * @param constants The constants object from the replay.
     * @param scale The scale factor the visualizer is using.
     * @param onSelect The callback for when this planet is selected.
     */
    constructor(constants, scale, onSelect) {
        this.container = null;
        this.scale = scale;
        this.constants = constants;
        this.prev_time = 0;

        const pixelsPerUnit = assets.CELL_SIZE * scale;
        this.core = new PIXI.Sprite.from(assets.FISH_IMAGE);

        // Fish can be different sizes

        this.core.width = this.core.height = Math.max(assets.MIN_FISH_SIZE * pixelsPerUnit,
                                                Math.random() * assets.MAX_FISH_SIZE * pixelsPerUnit);
        this.core.anchor.x = 0.5;
        this.core.anchor.y = 0.5;
        this.core.alpha = 1;

        // start in a random position
        let x = Math.random() * assets.VISUALIZER_SIZE;
        let y = Math.random() * assets.VISUALIZER_HEIGHT;
        this.core.position.x = this.scale * assets.CELL_SIZE * x;
        this.core.position.y = this.scale * assets.CELL_SIZE * y;

        // Also have a random starting velocity
        this.vel_magnitude = Math.random() * assets.MAX_FISH_SPEED;
        this.angle = Math.random() * 2 * Math.PI;
        this.vel_x = this.vel_magnitude * Math.cos(this.angle);
        this.vel_y = this.vel_magnitude * Math.sin(this.angle);

        // Orient the sprite in the direction that it's moving
        this.core.rotation = Math.atan2(this.vel_y, this.vel_x); + Math.PI / 2;

        this.core.interactive = true;
        this.core.buttonMode = true;
        this.core.on("pointerdown", (e) => {
            // When clicked, notify the visualizer
            onSelect("fish", {
                id: this.id,
            });
        });
    }

    /**
     * Add this factory to the PIXI stage.
     * @param container {PIXI.Container} The parent container of the fish
     * sprites.
     */
    attach(container) {
        container.addChild(this.core);
        this.container = container;
    }

    get id() {
        // TODO: do fish get IDs?
        return 0;
    }

    /**
     * Update the fish display based on the current frame and time.
     * @param dt
     */
    update(time, dt) {
        let time_delta = time - this.prev_time;

        // change angle and velocity a bit each shift
        let delta = Math.random();
        let vel_shift = delta - 0.5;
        this.vel_magnitude += vel_shift;
        this.angle += (delta - 0.5) * Math.PI / 30;
        this.vel_x = this.vel_magnitude * Math.cos(this.angle);
        this.vel_y = this.vel_magnitude * Math.sin(this.angle);

        // Orient the sprite in the direction that it's moving
        this.core.rotation = Math.atan2(this.vel_y, this.vel_x); + Math.PI / 2;

        // Move the sprite forward, wrap around screen as needed
        let x = (this.core.position.x + time_delta * this.vel_x) % assets.VISUALIZER_SIZE;
        let y = (this.core.position.y + time_delta * this.vel_y) % assets.VISUALIZER_HEIGHT;

        if (x < 0) {
            x += assets.VISUALIZER_SIZE;
        }
        if (y < 0) {
            y += assets.VISUALIZER_HEIGHT;
        }

        this.core.position.x = x;
        this.core.position.y = y;

    }
}