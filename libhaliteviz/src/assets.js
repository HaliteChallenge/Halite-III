const PIXI = require("pixi.js");

export let ASSET_ROOT = "dist/";

export const BASE_VISUALIZER_WIDTH = 600;
export const BASE_VISUALIZER_HEIGHT = 600;
export const STATS_SIZE = 20;
export const CELL_SIZE = 1;

export const PLAYER_COLORS = [0x00a54e, 0x9c781f, 0x850dff, 0xb24f0b,
                              0xff0000, 0xbd00db, 0xFFFF00, 0x009900,
                              0xFF8C00, 0xff0000, 0x00FF00, 0xBA55D3,
                              0xffae42, 0xbd00db, 0xFF69B4, 0xFFFF00];
export const EXPLOSION_COLOR = 0xb7b7b7;
export const PLANET_COLOR = 0xb7b7b7;
export const FISH_COLOR = 0xFFA500;

export const SPRITE_COLOR = 0xFFFFFF;
export const SPRITE_ALPHA = 0.8;
export const FACTORY_BASE_COLOR = 0xFFFFFF;
export const FACTORY_BASE_ALPHA = 0.8;


export const MAX_PRODUCTION = 1000;
export const MAP_COLOR_LIGHT = 0x00FFFF;
export const MAP_COLOR_MEDIUM = 0xFF00FF;
export const MAP_COLOR_DARK = 0xFFFF00;
export const MAP_ALPHA = 0.1;
export const OWNER_TINT_ALPHA = 0.25;
export const MAP_SQUARE_SIZE = 10;
export const LINE_COLOR = 0x000000;
export const LINE_WIDTH = 1;
export const DRAW_LINES_BASE_MAP = true;
export const DRAW_LINES_OWNER_MAP = false;


export const MIN_FISH_SIZE = 1;
export const MAX_FISH_SIZE = 15;
export const MAX_FISH_SPEED = 5;

export let PLANET_EXPLOSION_SHEET = null;
export let SHIP_EXPLOSION_SHEET = null;
export let TURTLE_SPRITES = [];


function loadSpritesheet(meta, textureImage) {
    return new Promise((resolve) => {
        const texture = PIXI.BaseTexture.fromImage(textureImage);
        const sheet = new PIXI.Spritesheet(texture, meta);
        sheet.parse(() => {
            resolve(sheet);
        });
    });
}

/**
 * Preload all assets, avoiding a hang when they are used for the first time.
 *
 * @param {PIXI.prepare.BasePrepare} prepare
 */
export function prepareAll(renderer, prepare) {
    // Right thing is prepare.add, but that does't seem to work.
    // Possibly related to https://github.com/pixijs/pixi.js/issues/3829
    // Workaround: https://stackoverflow.com/a/42762277

    if (!renderer.bindTexture) return;

    Object.keys(SHIP_EXPLOSION_SHEET.data.frames)
        .forEach((frame) => renderer.bindTexture(PIXI.Texture.from(frame).baseTexture));

    Object.keys(PLANET_EXPLOSION_SHEET.data.frames)
        .forEach((frame) => renderer.bindTexture(PIXI.Texture.from(frame).baseTexture));
}

export function setAssetRoot(path) {
    if (ASSET_ROOT === path) {
        // Bail if asset root already set
        return Promise.resolve();
    }

    // TODO: clear PIXI.loader to avoid exception

    ASSET_ROOT = path;

    return Promise.all([
        new Promise((resolve) => {
            PIXI.loader.add("turtle1", require("../assets/p1.png"))
                .add("turtle2", require("../assets/p2.png"))
                .add("turtle3", require("../assets/p3.png"))
                .add("turtle4", require("../assets/p4.png"))
                .load((loader, resources) => {
                    TURTLE_SPRITES.push(resources.turtle1.texture);
                    TURTLE_SPRITES.push(resources.turtle2.texture);
                    TURTLE_SPRITES.push(resources.turtle3.texture);
                    TURTLE_SPRITES.push(resources.turtle4.texture);
                    resolve();
                });
        }),
        loadSpritesheet(
            require("../assets/planet-explosion.json"),
            ASSET_ROOT + require("../assets/planet-explosion.png"),
        ).then((sheet) => {
            PLANET_EXPLOSION_SHEET = sheet;
        }),
        loadSpritesheet(
            require("../assets/ship-explosion.json"),
            ASSET_ROOT + require("../assets/ship-explosion.png"),
        ).then((sheet) => {
            SHIP_EXPLOSION_SHEET = sheet;
        }),
    ]);
}
