import theme from "./theme";

const PIXI = require("pixi.js");

export let ASSET_ROOT = "dist/";

export const BASE_VISUALIZER_WIDTH = 600;
export const BASE_VISUALIZER_HEIGHT = 600;
export const STATS_SIZE = 20;
export const CELL_SIZE = 1;

export const PLAYER_COLORS = [0x1BB15A, 0x966AFB, 0xF54356, 0xFABB2C,
                              0xff0000, 0xbd00db, 0xFFFF00, 0x009900,
                              0xFF8C00, 0xff0000, 0x00FF00, 0xBA55D3,
                              0xffae42, 0xbd00db, 0xFF69B4, 0xFFFF00];
export const EXPLOSION_COLOR = 0xb7b7b7;
export const PLANET_COLOR = 0xb7b7b7;

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

export let PLANET_EXPLOSION_SHEET = null;
export let SHIP_EXPLOSION_SHEET = null;
export let TURTLE_SPRITES = [];
export let BASE_SPRITES = [];
export let HALO_SPRITE = null;


function loadSpritesheet(meta, textureImage) {
    return new Promise((resolve) => {
        const texture = PIXI.BaseTexture.fromImage(textureImage, 'use-credentials');
        const sheet = new PIXI.Spritesheet(texture, meta);
        texture.on('loaded', () => {
            sheet.parse(() => {
                resolve(sheet);
            });
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
        theme.name === "halite2" ? loadSpritesheet(
            require("../assets/planet-small.json"),
            ASSET_ROOT + require("../assets/planet-small.png"),
        ).then((sheet) => {
            BASE_SPRITES.push(sheet.textures["CoreSmall.png"]);
            BASE_SPRITES.push(sheet.textures["CoreSmall.png"]);
            BASE_SPRITES.push(sheet.textures["CoreSmall.png"]);
            BASE_SPRITES.push(sheet.textures["CoreSmall.png"]);
        }) : Promise.resolve(),

        new Promise((resolve) => {
            PIXI.loader.add("halo", require("../assets/halo2.png"))
                .add("base_green", require("../assets/base-green.png"))
                .add("base_purple", require("../assets/base-purple.png"))
                .add("base_red", require("../assets/base-red.png"))
                .add("base_yellow", require("../assets/base-yellow.png"))
                .load((loader, resources) => {
                    HALO_SPRITE = resources.halo.texture;
                    if (theme.name === "shipyard") {
                        BASE_SPRITES.push(resources.base_green.texture);
                        BASE_SPRITES.push(resources.base_purple.texture);
                        BASE_SPRITES.push(resources.base_red.texture);
                        BASE_SPRITES.push(resources.base_yellow.texture);
                    }
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
        loadSpritesheet(
            require("../assets/turtle.json"),
            ASSET_ROOT + require("../assets/turtle.png"),
        ).then((sheet) => {
            TURTLE_SPRITES.push([
                sheet.textures["turtle-green-low.png"],
                sheet.textures["turtle-green-medium.png"],
                sheet.textures["turtle-green-high.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["turtle-purple-low.png"],
                sheet.textures["turtle-purple-medium.png"],
                sheet.textures["turtle-purple-high.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["turtle-red-low.png"],
                sheet.textures["turtle-red-medium.png"],
                sheet.textures["turtle-red-high.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["turtle-yellow-low.png"],
                sheet.textures["turtle-yellow-medium.png"],
                sheet.textures["turtle-yellow-high.png"],
            ]);
        }),
    ]);
}
