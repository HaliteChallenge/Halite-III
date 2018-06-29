const PIXI = require("pixi.js");

export let ASSET_ROOT = "dist/";

export const VISUALIZER_SIZE = 700;
export const VISUALIZER_HEIGHT = 700;
export const STATS_SIZE = 20;
export const CELL_SIZE = 1;
export const PLAYER_COLORS = [0xff0000, 0x00FF00, 0xBA55D3, 0xffae42, 0xbd00db, 0xFF69B4, 0xFFFF00, 0x009900, 0xFF8C00, 0xff0000, 0x00FF00, 0xBA55D3, 0xffae42, 0xbd00db, 0xFF69B4, 0xFFFF00, 0x009900, 0xFF8C00];
export const PLANET_COLOR = 0xb7b7b7
export const FISH_COLOR = 0xFFA500;
export const HEALTH_BAR_COLOR = 0x990000;
export const EXPLOSION_COLOR = 0xFF7607;

export const SPRITE_COLOR = 0xFFFFFF;
export const SPRITE_ALPHA = 0.8;
export const HALO_COLOR = 0XD0D0D0;
export const HALO_ALPHA = 0.8;
export const MAX_PRODUCTION = 255;
export const MAP_COLOR_LIGHT = 0x00FFFF;
export const MAP_COLOR_MEDIUM = 0x0000FF;
export const MAP_COLOR_DARK = 0x0000A0;
export const MAP_ALPHA = 0.1;
export const OWNER_TINT_ALPHA = 0.4;
export const MAP_SQUARE_SIZE = 10;
export const LINE_COLOR = 0x000000;
export const LINE_WIDTH = 1;
export const MIN_FISH_SIZE = 1;
export const MAX_FISH_SIZE = 15;
export const MAX_FISH_SPEED = 5;


export let BACKGROUND_IMAGES = [];
export let PLANET_IMAGE = null;
export let PLANET_HALO_IMAGE = null;
export let PLANET_IMAGE_SMALL = null;
export let PLANET_HALO_IMAGE_SMALL = null;
export let SHIP_IMAGE = "";
export let HALO_IMAGE = "";
export let EXHAUST_IMAGE = "";
export let FISH_IMAGE = "";

export let HALLOWEEN_PLANET_IMAGE = null;
export let WINTER_PLANET_IMAGE = null;
export let NEWYEAR_PLANET_IMAGE = null;

export let PLANET_SHEET = null;
export let PLANET_SHEET_SMALL = null;
export let PLANET_EXPLOSION_SHEET = null;
export let SHIP_EXPLOSION_SHEET = null;
export let SHIP_SHEET = null;
export let ATTACK_SHEET = null;
export let DOCKING_SHEET = null;
export let DOCKING_MIRRORED_SHEET = null;
export let TRACTOR_BEAM_SHEET = null;
export let TRACTOR_BEAM_FRAMES = null;

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

    BACKGROUND_IMAGES.forEach((image) => renderer.bindTexture(PIXI.Texture.from(image)));
}

export function setAssetRoot(path) {
    ASSET_ROOT = path;

    BACKGROUND_IMAGES = [
        ASSET_ROOT + require("../assets/backgrounds/Space001.png"),
    ];
    HALO_IMAGE = ASSET_ROOT + require("../assets/halo.png");
    FISH_IMAGE = ASSET_ROOT + require("../assets/fish.png");
    ATTACK_SHEET = loadSpritesheet(
        require("../assets/ship-battle.json"),
        ASSET_ROOT + require("../assets/ship-battle.png"),
        () => {}
    );

    PLANET_SHEET = loadSpritesheet(
        require("../assets/planet-obj.json"),
        ASSET_ROOT + require("../assets/planet-obj.png"),
        () => {
            PLANET_IMAGE = PIXI.Texture.fromFrame("Core.png");
            PLANET_HALO_IMAGE = PIXI.Texture.fromFrame("Ring.png");
        }
    );

    HALLOWEEN_PLANET_IMAGE = ASSET_ROOT + require("../assets/halloween-obj.png");
    WINTER_PLANET_IMAGE = ASSET_ROOT + require("../assets/winter-obj.png");
    NEWYEAR_PLANET_IMAGE = ASSET_ROOT + require("../assets/newyear-obj.png");

    PLANET_SHEET_SMALL = loadSpritesheet(
        require("../assets/planet-small.json"),
        ASSET_ROOT + require("../assets/planet-small.png"),
        () => {
            PLANET_IMAGE_SMALL = PIXI.Texture.fromFrame("CoreSmall.png");
            PLANET_HALO_IMAGE_SMALL = PIXI.Texture.fromFrame("RingSmall.png");
        }
    );

    PLANET_EXPLOSION_SHEET = loadSpritesheet(
        require("../assets/planet-explosion.json"),
        ASSET_ROOT + require("../assets/planet-explosion.png"),
        () => {}
    );

    SHIP_SHEET = loadSpritesheet(
        require("../assets/ship-neutral.json"),
        ASSET_ROOT + require("../assets/ship-neutral.png"),
        () => {
            SHIP_IMAGE = PIXI.Texture.fromFrame("ship.png");
            EXHAUST_IMAGE = PIXI.Texture.fromFrame("trail.png");
            HALO_IMAGE = PIXI.Texture.fromFrame("aura.png");
        }
    );

    SHIP_EXPLOSION_SHEET = loadSpritesheet(
        require("../assets/ship-explosion.json"),
        ASSET_ROOT + require("../assets/ship-explosion.png"),
        () => {}
    );

    DOCKING_SHEET = loadSpritesheet(
        require("../assets/Docking.json"),
        ASSET_ROOT + require("../assets/Docking.png"),
        () => {}
    );
    DOCKING_MIRRORED_SHEET = loadSpritesheet(
        require("../assets/DockingMirrored.json"),
        ASSET_ROOT + require("../assets/DockingMirrored.png"),
        () => {}
    );

    TRACTOR_BEAM_SHEET = loadSpritesheet(
        require("../assets/mining-bar.json"),
        ASSET_ROOT + require("../assets/mining-bar.png"),
        (sheet) => {
            TRACTOR_BEAM_FRAMES = Object.keys(sheet.data.frames)
                .sort()
                .map((frame) => {
                    return PIXI.Texture.fromFrame(frame);
                });
        }
    );
}
