import * as assets from "./assets";
import * as PIXI from "pixi.js";

export class FrameAnimation {
    constructor(start, duration, update, draw, finish) {
        this.start = start;
        this.duration = duration;
        this.update = update;
        this.draw = draw;
        this.finish = finish;
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

export class PlanetExplosionFrameAnimation extends SpritesheetFrameAnimation {
    constructor(event, start, duration, cellSize, container) {
        super({
            sheet: assets.PLANET_EXPLOSION_SHEET,
            sizeFactor: 8,
            x: event.location.x,
            y: event.location.y,
            tintColor: assets.PLAYER_COLORS[event.owner_id],
            start,
            duration,
            cellSize,
            container,
            opacity: 0.1,
        });
    }
}

export class ShipExplosionFrameAnimation extends SpritesheetFrameAnimation {
    constructor(location, color, start, duration, cellSize, container) {
        super({
            sheet: assets.SHIP_EXPLOSION_SHEET,
            sizeFactor: 4,
            x: location.x,
            y: location.y,
            tintColor: color,
            start,
            duration,
            cellSize,
            container,
        });
    }
}
