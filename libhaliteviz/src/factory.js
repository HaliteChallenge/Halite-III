import * as PIXI from "pixi.js";

import * as assets from "./assets";

/**
 * Manages a factory on screen.
 */
export class Factory {
    /**
     *
     * @param factoryBase The "base" factory record in the replay (contains a location)
     * @param constants The constants object from the replay.
     * @param scale The scale factor the visualizer is using.
     * @param onSelect The callback for when this factory is selected.
     */
    constructor(factoryBase, constants, scale, onSelect, renderer) {
        this.container = null;
        //this.overlay = null;
        this.factoryBase = factoryBase;
        this.scale = scale;
        this.constants = constants;

        const pixelsPerUnit = assets.CELL_SIZE * scale;
        // TODO: Use a factory image of some sort
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
        this.core.width = this.core.height = 2 * assets.CELL_SIZE * pixelsPerUnit;
        this.core.anchor.x = 0.5;
        this.core.anchor.y = 0.5;
        this.core.alpha = 0.9;

        this.core.position.x = scale * assets.CELL_SIZE * factoryBase.x
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

        const color = factoryStatus.owner === null ?
            assets.PLANET_COLOR : assets.PLAYER_COLORS[factoryStatus.owner];


        this.core.tint = color;
        this.core.interactive = true;
        this.core.buttonMode = true;

        //this.halo.visible = factoryStatus.health > 0;
        this.core.visible = factoryStatus.alive > 0;
    }
}


