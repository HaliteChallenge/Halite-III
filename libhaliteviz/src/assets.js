import * as theme from "./theme";

const PIXI = require("pixi.js");

export let ASSET_ROOT = "dist/";

export const BASE_VISUALIZER_WIDTH = 600;
export const BASE_VISUALIZER_HEIGHT = 600;
export const STATS_SIZE = 20;
export const CELL_SIZE = 1;

export const PLAYER_COLORS = theme.default().playerColors || [0x1BB15A, 0xF54356, 0xFABB2C, 0xE65598];
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

export let TURTLE_SPRITES = [];
export let BASE_SPRITES = [];
export let HALO_SPRITE = null;
export let MAP_SPRITE = null;
export let MAP_BORDER_SPRITE = null;

let SHEETS = [];

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

    SHEETS.forEach(
        sheet => Object.keys(sheet.data.frames)
            .forEach(frame => renderer.bindTexture(PIXI.Texture.from(frame).baseTexture)));
}

export function setAssetRoot(path) {
    if (ASSET_ROOT === path) {
        // Bail if asset root already set
        return Promise.resolve();
    }

    // TODO: clear PIXI.loader to avoid exception

    ASSET_ROOT = path;

    const promises = [];

    theme.applyTheme();

    promises.push(new Promise((resolve) => {
        PIXI.loader.add("halo", require("../assets/halo2.png"))
            .load((loader, resources) => {
                HALO_SPRITE = resources.halo.texture;
                resolve();
            });
    }));

    if (theme.selectedTheme === "Colorblind") {
        promises.push(loadSpritesheet(
            require("../assets/colorblind.json"),
            ASSET_ROOT + require("../assets/colorblind.png"),
        ).then((sheet) => {
            SHEETS.push(sheet);

            TURTLE_SPRITES.push([
                sheet.textures["turtle-cb-0.png"],
                sheet.textures["turtle-cb-0.png"],
                sheet.textures["turtle-cb-0.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["turtle-cb-1.png"],
                sheet.textures["turtle-cb-1.png"],
                sheet.textures["turtle-cb-1.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["turtle-cb-2.png"],
                sheet.textures["turtle-cb-2.png"],
                sheet.textures["turtle-cb-2.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["turtle-cb-3.png"],
                sheet.textures["turtle-cb-3.png"],
                sheet.textures["turtle-cb-3.png"],
            ]);
            BASE_SPRITES.push(sheet.textures["base-cb-0.png"]);
            BASE_SPRITES.push(sheet.textures["base-cb-1.png"]);
            BASE_SPRITES.push(sheet.textures["base-cb-2.png"]);
            BASE_SPRITES.push(sheet.textures["base-cb-3.png"]);
        }));
    }
    else if (theme.selectedTheme === "Halite II") {
        promises.push(loadSpritesheet(
            require("../assets/planet-small.json"),
            ASSET_ROOT + require("../assets/planet-small.png"),
        ).then((sheet) => {
            SHEETS.push(sheet);

            BASE_SPRITES.push(sheet.textures["CoreSmall.png"]);
            BASE_SPRITES.push(sheet.textures["CoreSmall.png"]);
            BASE_SPRITES.push(sheet.textures["CoreSmall.png"]);
            BASE_SPRITES.push(sheet.textures["CoreSmall.png"]);
        }));

        promises.push(loadSpritesheet(
            require("../assets/ship-neutral.json"),
            ASSET_ROOT + require("../assets/ship-neutral.png"),
        ).then((sheet) => {
            SHEETS.push(sheet);

            TURTLE_SPRITES.push([
                sheet.textures["ship.png"],
                sheet.textures["ship.png"],
                sheet.textures["ship.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["ship.png"],
                sheet.textures["ship.png"],
                sheet.textures["ship.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["ship.png"],
                sheet.textures["ship.png"],
                sheet.textures["ship.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["ship.png"],
                sheet.textures["ship.png"],
                sheet.textures["ship.png"],
            ]);
        }));
    }
    else if (theme.selectedTheme === "Roguelike") {
        promises.push(loadSpritesheet(
            require("../assets/nethack.json"),
            ASSET_ROOT + require("../assets/nethack.png"),
        ).then((sheet) => {
            SHEETS.push(sheet);

            MAP_SPRITE = sheet.textures["lcurly.png"];
            MAP_BORDER_SPRITE = sheet.textures["hash.png"];

            TURTLE_SPRITES.push([
                sheet.textures["at.png"],
                sheet.textures["at.png"],
                sheet.textures["at.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["at.png"],
                sheet.textures["at.png"],
                sheet.textures["at.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["at.png"],
                sheet.textures["at.png"],
                sheet.textures["at.png"],
            ]);
            TURTLE_SPRITES.push([
                sheet.textures["at.png"],
                sheet.textures["at.png"],
                sheet.textures["at.png"],
            ]);
            BASE_SPRITES.push(sheet.textures["lt.png"]);
            BASE_SPRITES.push(sheet.textures["lt.png"]);
            BASE_SPRITES.push(sheet.textures["lt.png"]);
            BASE_SPRITES.push(sheet.textures["lt.png"]);
            BASE_SPRITES.push(sheet.textures["gt.png"]);
            BASE_SPRITES.push(sheet.textures["gt.png"]);
            BASE_SPRITES.push(sheet.textures["gt.png"]);
            BASE_SPRITES.push(sheet.textures["gt.png"]);
        }));
    }
    else {
        promises.push(loadSpritesheet(
            require("../assets/turtle.json"),
            ASSET_ROOT + require("../assets/turtle.png"),
        ).then((sheet) => {
            SHEETS.push(sheet);

            BASE_SPRITES.push(sheet.textures["base-green.png"]);
            BASE_SPRITES.push(sheet.textures["base-red.png"]);
            BASE_SPRITES.push(sheet.textures["base-yellow.png"]);
            BASE_SPRITES.push(sheet.textures["base-purple.png"]);

            TURTLE_SPRITES.push([
                sheet.textures["turtle-green-low.png"],
                sheet.textures["turtle-green-medium.png"],
                sheet.textures["turtle-green-high.png"],
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
            TURTLE_SPRITES.push([
                sheet.textures["turtle-purple-low.png"],
                sheet.textures["turtle-purple-medium.png"],
                sheet.textures["turtle-purple-high.png"],
            ]);
        }));
    }

    return Promise.all(promises);
}
