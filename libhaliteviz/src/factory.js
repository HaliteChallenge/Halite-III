import * as PIXI from "pixi.js";

import * as assets from "./assets";


const WINTER_START = new Date(2018, 11, 23);
const NEW_YEAR_START = new Date(2018, 11, 30);
const NEW_YEAR_END = new Date(2019, 0, 2);

export function holidaySprite() {
    const date = new Date();
    if (date.getTime() >= WINTER_START.getTime() && date.getTime() < NEW_YEAR_START.getTime()) {
        return assets.WINTER_PLANET_IMAGE;
    }
    else if (date.getTime() >= NEW_YEAR_START.getTime() && date.getTime() < NEW_YEAR_END.getTime()) {
        return assets.NEWYEAR_PLANET_IMAGE;
    }
    return null;
}

function useHolidaySprite() {
    if (window.localStorage["holiday"] === "false") return false;
    // Cast result of holidaySprite to Boolean
    return !!holidaySprite();
}


/**
 * Manages a planet on screen.
 */
export class Factory {
    /**
     *
     * @param factoryBase The "base" factory record in the replay. (replay.factory[i])
     * @param constants The constants object from the replay.
     * @param scale The scale factor the visualizer is using.
     * @param onSelect The callback for when this planet is selected.
     */
    constructor(factoryBase, constants, scale, onSelect, renderer) {
        this.container = null;
        //this.overlay = null;
        this.factoryBase = factoryBase;
        this.scale = scale;
        this.constants = constants;

        const pixelsPerUnit = assets.CELL_SIZE * scale;
        // TODO: Use a factory image of some sort
        // Make a sprite a circle
        const factoryShape = new PIXI.Graphics();
        factoryShape.beginFill(assets.HALO_COLOR, assets.HALO_ALPHA);
        // draw rect - x coord, y coord, pts, radius
        factoryShape.drawRect(0, 0, assets.CELL_SIZE * 40, assets.CELL_SIZE * 30);
        factoryShape.endFill();
        let factoryTexture = renderer.generateTexture(factoryShape);
        this.core = new PIXI.Sprite(factoryTexture);
        //this.halo = PIXI.Sprite.from(assets.PLANET_HALO_IMAGE);
        // Center of sprite != center of circle
        //this.halo.anchor.x = (18 + 36) / 104;
        //this.halo.anchor.y = (18 + 36) / 108;
        //this.halo.alpha = 0.2;
        // Scale halo such that inner circular area represents docking radius
        // TODO: determine what this looks like visually and if its still useful
        //this.halo.width = this.halo.height = 1.4 * (207 / 167) * 2 * (factoryBase.x) * pixelsPerUnit;
        //this.baseHaloAlpha = 0.2;

        // TODO: make not about r
        this.core.width = this.core.height = 2 * factoryBase.r * pixelsPerUnit;
        this.core.anchor.x = 0.5;
        this.core.anchor.y = 0.5;
        this.core.alpha = 0.9;

        this.core.position.x = scale * assets.CELL_SIZE * factoryBase.x;
        this.core.position.y = scale * assets.CELL_SIZE * factoryBase.y;
        //this.halo.position.x = scale * assets.CELL_SIZE * factoryBase.x;
        //this.halo.position.y = scale * assets.CELL_SIZE * factoryBase.y;

        // Rotate the core a bit just to mix it up
        this.core.rotation = Math.random() * 2 * Math.PI;
        this.core.interactive = true;
        this.core.buttonMode = true;
        this.core.on("pointerdown", (e) => {
            // When clicked, notify the visualizer
            // TODO: switch to factory
            onSelect("planet", {
                id: this.id,
            });
        });
    }

    /**
     * Add this factory to the PIXI stage.
     * @param container {PIXI.Container} The parent container of the factory
     * sprites.
     * @param overlay {PIXI.Graphics} A graphics object used to draw overlays.
     */
    attach(container) {
        container.addChild(this.core);
        this.container = container;
        //this.overlay = overlay;
    }

    get id() {
        return this.factoryBase.id;
    }

    /**
     * Update the factory display based on the current frame and time.
     * @param factoryStatus
     * @param dt
     */
     // TODO: make above comment more sensical
    update(factoryStatus, dt) {
        // TODO: should always be owned - make transparency proportional to production?
        if (factoryStatus.owner !== null) {
            this.core.alpha = 1.0;
            // this.halo.rotation += dt / 400;
            // if (this.halo.rotation > 2 * Math.PI) 
            //     this.halo.rotation -= 2 * Math.PI;
        }
        else {
            //this.halo.alpha = this.baseHaloAlpha;
            this.core.alpha = 0.5;
        }
        // randomly rotate and move
        // let rand = Math.random();
        // if (rand > 0.5) {
        //     this.core.rotation += dt / 400;
        //     if (this.core.rotation > 2 * Math.PI)
        //         this.core.rotation -= 2 * Math.PI;
        // }
        // // make planets moving fish.
        // // TODO: double check this works, modify to just be random fish swarms
        // let delta = (rand * 10) - 5;
        // this.core.position.x += delta;
        // this.core.position.y += delta;

        const side = assets.CELL_SIZE * this.scale;
        const color = factoryStatus.owner === null ?
            assets.PLANET_COLOR : assets.PLAYER_COLORS[factoryStatus.owner];

        // TODO: change from r when no longer getting in planet data
        const radius = side * this.factoryBase.r;

        // TODO: make this about production ability instead?
        const health_factor = factoryStatus.health / this.factoryBase.health;
        const health_bar = health_factor * 2 * radius;

        this.core.tint = color;
        this.core.interactive = true;
        this.core.buttonMode = true;

        if (factoryStatus.health === 0) {
            this.core.interactive = false;
            this.core.buttonMode = false;
        }

        //this.halo.visible = factoryStatus.health > 0;
        this.core.visible = factoryStatus.health > 0;
    }
}


