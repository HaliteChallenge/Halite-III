import * as assets from "./assets";
import * as PIXI from "pixi.js";
import {ShockwaveFilter} from "@pixi/filter-shockwave";

export class FrameAnimation {
    constructor(start, duration, update, draw, finish) {
        this.start = start;
        this.duration = duration;
        this.update = update;
        this.draw = draw;
        this.finish = finish;
    }
}

export class SpawnAnimation extends FrameAnimation {
    constructor({ event, frame, duration, cellSize, container, reverse=false }) {
        const filter = new ShockwaveFilter();
        filter.radius = 30;
        filter.amplitude = 5;
        filter.wavelength = 7.5;
        filter.brightness = 1.25;
        if (!container.filters) {
            container.filters = [filter];
        }
        else {
            container.filters = container.filters.concat([filter]);
        }

        super(frame, duration, () => {
        }, (camera, frameTime) => {
            // Cubic ease in out
            let t = 1 - ((duration - frameTime) / duration);
            t = - t * (t - 2);
            filter.time = t;
            const [ x, y ] = camera.worldToCamera(event.location.x + 0.5, event.location.y + 0.5);
            filter.center = [ x * cellSize * camera.scale,
                              y * cellSize * camera.scale ];
        }, () => {
            container.filters = container.filters.filter(x => x !== filter);
        });
    }
}

export class SpritesheetFrameAnimation extends FrameAnimation {
    constructor(options) {
        const start = options.start;
        const duration = options.duration || 2;
        const sheet = options.sheet;
        const x = options.x;
        const y = options.y;
        const sizeFactor = options.sizeFactor;
        const tintColor = options.tintColor;
        const cellSize = options.cellSize;
        const container = options.container;
        const opacity = options.opacity || 1.0;

        let frames = [];
        for (let frame of Object.keys(sheet.data.frames).sort()) {
            frames.push(PIXI.Texture.fromFrame(frame));
        }

        let sprite = PIXI.Sprite.from(frames[0]);
        sprite.anchor.x = 0.5;
        sprite.anchor.y = 0.5;
        sprite.position.x = cellSize * x + cellSize / 2;
        sprite.position.y = cellSize * y + cellSize / 2;
        sprite.width = cellSize * sizeFactor;
        sprite.height = cellSize * sizeFactor;
        sprite.tint = tintColor;
        sprite.alpha = opacity;
        sprite.blendMode = PIXI.BLEND_MODES.SCREEN;
        container.addChild(sprite);

        super(start, duration, () => {

        }, (camera, frameTime) => {
            const t = (duration - frameTime) / duration;
            const index = Math.floor(t * (frames.length - 1));
            sprite.width = cellSize * sizeFactor * camera.scale;
            sprite.height = cellSize * sizeFactor * camera.scale;
            const [ cellX, cellY ] = camera.worldToCamera(x, y);
            sprite.position.x = cellSize * cellX * camera.scale + cellSize * camera.scale / 2;
            sprite.position.y = cellSize * cellY * camera.scale + cellSize * camera.scale / 2;
            sprite.texture = frames[index];
            sprite.alpha = frameTime / duration;
        }, () => {
            container.removeChild(sprite);
        });
    }
}
