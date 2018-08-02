import * as PIXI from "pixi.js";

import * as assets from "./assets";

export class Dropoff {
    /**
     *
     * @param factoryBase The "base" factory record in the replay (contains a location)
     * @param constants The constants object from the replay.
     * @param scale The scale factor the visualizer is using.
     * @param onSelect The callback for when this factory is selected.
     * @param renderer The renderer from the visualizer, used for rendering factories
     */
    constructor(visualizer, factoryBase, constants, scale, onSelect, renderer) {
        this.visualizer = visualizer;
        this.container = null;
        this.factoryBase = factoryBase;
        this.scale = scale;
        this.constants = constants;
        this.owner = factoryBase.owner;

        const pixelsPerUnit = assets.CELL_SIZE * scale;

        // TODO: Switch factory from square to image
        this.makeCore(renderer, pixelsPerUnit);

        // Rotate factories, make twice the size of a cell
        this.core.width = this.core.height = 2 * pixelsPerUnit;
        this.highlight.width = this.highlight.height = 2.25 * pixelsPerUnit;
        // anchor factory in center for rotation
        this.core.anchor.x = 0.5;
        this.core.anchor.y = 0.5;
        this.core.alpha = 0.7;
        this.highlight.anchor.x = 0.5;
        this.highlight.anchor.y = 0.5;
        this.highlight.visible = false;

        // Place factories in center of cells
        this.core.position.x = pixelsPerUnit * factoryBase.x + pixelsPerUnit / 2;
        this.core.position.y = pixelsPerUnit * factoryBase.y + pixelsPerUnit / 2;
        this.highlight.position.copy(this.core.position);

        // Rotate the factory a bit just to mix it up
        this.core.rotation = Math.random() * 2 * Math.PI;
        this.core.tint = assets.PLAYER_COLORS[this.owner];
    }

    makeCore(renderer, pixelsPerUnit) {
        const factoryShape = new PIXI.Graphics();
        factoryShape.beginFill(assets.FACTORY_BASE_COLOR, assets.FACTORY_BASE_ALPHA);
        factoryShape.drawRect(0, 0, pixelsPerUnit, pixelsPerUnit);
        factoryShape.endFill();
        let factoryTexture = renderer.generateTexture(factoryShape);
        this.core = new PIXI.Sprite(factoryTexture);
        this.highlight = new PIXI.Sprite(factoryTexture);
    }

    /**
     * Add this factory to the PIXI stage.
     * @param container {PIXI.Container} The parent container of the factory sprites.
     */
    attach(container) {
        container.addChild(this.highlight);
        container.addChild(this.core);
        this.container = container;
    }

    /**
     * Update the factory display based on the current frame and time.
     */
    //TODO: update to make factories change color when player dies
    update(delta=1) {
    }

    destroy() {
        this.container.removeChild(this.core);
        this.container.removeChild(this.highlight);
        this.container = null;
    }

    draw() {
        const pixelsPerUnit = assets.CELL_SIZE * this.visualizer.camera.scale;
        this.core.width = this.core.height = 1.5 * pixelsPerUnit;
        this.core.rotation = (((this.visualizer.frame + this.visualizer.time) % 20) / 20) * 2 * Math.PI;

        // Account for camera panning
        const [ cellX, cellY ] = this.visualizer.camera.worldToCamera(
            this.factoryBase.x,
            this.factoryBase.y
        );

        this.core.position.x = pixelsPerUnit * cellX + pixelsPerUnit / 2;
        this.core.position.y = pixelsPerUnit * cellY + pixelsPerUnit / 2;

        this.highlight.position.copy(this.core.position);
        this.highlight.width = this.highlight.height = 1.75 * pixelsPerUnit;

        const camera = this.visualizer.camera;
        if (camera.selected && camera.selected.type === "point" &&
            camera.selected.x === this.factoryBase.x &&
            camera.selected.y === this.factoryBase.y) {
            this.highlight.visible = true;
            this.highlight.rotation = this.core.rotation;
        }
        else {
            this.highlight.visible = false;
        }
    }
}

/**
 * Manages a factory on screen.
 */
export class Factory extends Dropoff {
    constructor(...args) {
        super(...args);
        this.core.alpha = 0.9;
    }

    makeCore(renderer, pixelsPerUnit) {
        const factoryShape = new PIXI.Graphics();
        factoryShape.beginFill(assets.FACTORY_BASE_COLOR, assets.FACTORY_BASE_ALPHA);
        factoryShape.drawRect(0, 0, pixelsPerUnit, pixelsPerUnit);
        factoryShape.beginFill(0x000000, assets.FACTORY_BASE_ALPHA);
        factoryShape.drawRect(pixelsPerUnit / 4, pixelsPerUnit / 4,
                              pixelsPerUnit / 2, pixelsPerUnit / 2);
        factoryShape.endFill();
        let factoryTexture = renderer.generateTexture(factoryShape);
        this.core = new PIXI.Sprite(factoryTexture);
        this.highlight = new PIXI.Sprite(factoryTexture);
    }

    draw() {
        super.draw();
        const pixelsPerUnit = assets.CELL_SIZE * this.visualizer.camera.scale;
        const energy = this.visualizer.frame === 0 ? this.visualizer.replay.players[this.owner].energy : this.visualizer.currentFrame.energy[this.owner];
        const energyFactor = energy / 10000;
        this.core.width = this.core.height = (1.25 + Math.min(1.25, energyFactor)) * pixelsPerUnit;
        this.highlight.width = this.highlight.height = 1.25 * this.core.width;
    }
}
