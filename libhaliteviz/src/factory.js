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
     * @param renderer The renderer from the visualizer, used for rendering factories
     */
    constructor(factoryBase, constants, scale, onSelect, renderer) {
        this.container = null;
        //this.overlay = null;
        this.factoryBase = factoryBase;
        this.scale = scale;
        this.constants = constants;
        this.owner = factoryBase.owner;

        const pixelsPerUnit = assets.CELL_SIZE * scale;

        // TODO: Switch factory from square to image
        const factoryShape = new PIXI.Graphics();
        factoryShape.beginFill(assets.FACTORY_BASE_COLOR, assets.FACTORY_BASE_ALPHA);
        factoryShape.drawRect(0, 0, pixelsPerUnit, pixelsPerUnit);
        factoryShape.endFill();
        let factoryTexture = renderer.generateTexture(factoryShape);
        this.core = new PIXI.Sprite(factoryTexture);

        // Rotate factories, make twice the size of a cell
        this.core.width = this.core.height = 2 * pixelsPerUnit;
        // anchor factory in center for rotation
        this.core.anchor.x = 0.5;
        this.core.anchor.y = 0.5;
        this.core.alpha = 0.9;

        // Place factories in center of cells
        this.core.position.x = pixelsPerUnit * factoryBase.x + pixelsPerUnit / 2;
        this.core.position.y = pixelsPerUnit * factoryBase.y + pixelsPerUnit / 2;

        // Rotate the factory a bit just to mix it up
        this.core.rotation = Math.random() * 2 * Math.PI;
        this.core.interactive = true;
        this.core.buttonMode = true;
        this.core.on("pointerdown", (e) => {
            // When clicked, notify the visualizer
            // TODO: switch to factory (unified naming change with vue code)
            onSelect("planet", {
                id: this.owner,
            });
        });

        this.core.tint = assets.PLAYER_COLORS[this.owner];
    }

    /**
     * Add this factory to the PIXI stage.
     * @param container {PIXI.Container} The parent container of the factory sprites.
     */
    attach(container) {
        container.addChild(this.core);
        this.container = container;
    }

    get id() {
        return this.owner;
    }

    /**
     * Update the factory display based on the current frame and time.
     */
    //TODO: update to make factories change color when player dies
    update() {
        const pixelsPerUnit = assets.CELL_SIZE * this.scale;
        this.core.width = this.core.height = 2 * pixelsPerUnit;
        this.core.position.x = pixelsPerUnit * this.factoryBase.x + pixelsPerUnit / 2;
        this.core.position.y = pixelsPerUnit * this.factoryBase.y + pixelsPerUnit / 2;
    }
}
