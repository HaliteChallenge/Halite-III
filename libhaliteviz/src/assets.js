const PIXI = require("pixi.js");

export let ASSET_ROOT = "dist/";

export const BASE_VISUALIZER_WIDTH = 700;
export const BASE_VISUALIZER_HEIGHT = 700;
export const STATS_SIZE = 20;
export const CELL_SIZE = 1;

export const PLAYER_COLORS = [0xff0000, 0x00FF00, 0xBA55D3, 0xffae42,
                                0xFF69B4, 0xbd00db, 0xFFFF00, 0x009900,
                                0xFF8C00, 0xff0000, 0x00FF00, 0xBA55D3,
                                0xffae42, 0xbd00db, 0xFF69B4, 0xFFFF00];
export const EXPLOSION_COLOR = 0xb7b7b7;
export const PLANET_COLOR = 0xb7b7b7;
export const FISH_COLOR = 0xFFA500;

export const SPRITE_COLOR = 0xFFFFFF;
export const SPRITE_ALPHA = 0.8;
export const FACTORY_BASE_COLOR = 0xFFFFFF;
export const FACTORY_BASE_ALPHA = 0.8;


export const MAX_PRODUCTION = 255;
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
export let FISH_IMAGE = "";


export let PLANET_EXPLOSION_SHEET = null;
export let SHIP_EXPLOSION_SHEET = null;


function loadSpritesheet(meta, textureImage, onload) {
    const texture = PIXI.BaseTexture.fromImage(textureImage);
    const sheet = new PIXI.Spritesheet(texture, meta);
    sheet.parse(() => {
        onload(sheet);
    });
    return sheet;
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
    ASSET_ROOT = path;

    FISH_IMAGE = ASSET_ROOT + require("../assets/fish.png");

    PLANET_EXPLOSION_SHEET = loadSpritesheet(
        require("../assets/planet-explosion.json"),
        ASSET_ROOT + require("../assets/planet-explosion.png"),
        () => {}
    );

    SHIP_EXPLOSION_SHEET = loadSpritesheet(
        require("../assets/ship-explosion.json"),
        ASSET_ROOT + require("../assets/ship-explosion.png"),
        () => {}
    );
}
