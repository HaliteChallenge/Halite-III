import * as PIXI from "pixi.js";
import * as d3ScaleChromatic from "d3-scale-chromatic";
import {AdvancedBloomFilter} from '@pixi/filter-advanced-bloom';

import * as assets from "./assets";
import {PLAYER_COLORS} from "./assets";
import colorTheme from "./colors";
import theme from "./theme";

const themeParams = theme();
const [ _, COLOR_SCALE ] = colorTheme();

/**
 * Manages a Map on screen.
 */
export class Map {
    /**
     * @param replay The replay json
     * @param constants The constants object from the replay.
     * @param camera The camera.
     * @param onSelect The callback for when this planet is selected.
     * @param renderer The renderer used by the visualizer (for rendering the map)
     */
    constructor(replay, constants, camera, onSelect, renderer) {
        this.container = null;
        this.camera = camera;
        this.constants = constants;
        this.prev_time = 0;
        this.renderer = renderer;

        this.hovered = null;

        // Background image, based on initial production values
        // draw a map, with color intensity dependent on production value
        this.rows = replay.production_map.height;
        this.cols = replay.production_map.width;

        // Read in production values from replay file and store
        this.productions = new Array(this.rows);
        this.owners = null;
        for (let row = 0; row < this.rows; row++) {
            this.productions[row] = new Array(this.cols);
            for (let col = 0; col < this.cols; col++) {
                let cell = replay.production_map.grid[row][col];
                this.productions[row][col] = cell.energy;
            }
        }

        // Cache old production values so we can scrub backwards in
        // time when needed
        this.productionPatches = {};

        this.tintMap = new PIXI.particles.ParticleContainer(this.rows * this.cols, {
            vertices: true,
            position: true,
            tint: true,
        });

        // Generate the texture for a single map cell (16x16 white
        // square with a 2 pixel 70% black border blended on top)
        // Could probably be replaced with a real texture
        let borderWidth = 0;
        let textureWidth = 16;
        let g = new PIXI.Graphics();
        g.beginFill(0xFFFFFF, 1.0);
        g.drawRect(0, 0, textureWidth, textureWidth);
        // Be careful not to overlap when drawing the border, or else
        // some parts will be darker than others
        g.beginFill(0x000000, 0.5);
        g.drawRect(0, 0, textureWidth, borderWidth);
        g.drawRect(0, borderWidth, borderWidth, textureWidth - borderWidth);
        g.drawRect(textureWidth - borderWidth, borderWidth, borderWidth, textureWidth - borderWidth);
        g.drawRect(borderWidth, textureWidth - borderWidth, textureWidth - 2*borderWidth, borderWidth);
        const normalTex = renderer.generateTexture(g);

        borderWidth = 3;
        g = new PIXI.Graphics();
        // Be careful not to overlap when drawing the border, or else
        // some parts will be darker than others
        g.beginFill(0xFFFFFF, 1);
        g.drawRect(0, 0, textureWidth, borderWidth);
        g.drawRect(0, borderWidth, borderWidth, textureWidth - borderWidth);
        g.drawRect(textureWidth - borderWidth, borderWidth, borderWidth, textureWidth - borderWidth);
        g.drawRect(borderWidth, textureWidth - borderWidth, textureWidth - 2*borderWidth, borderWidth);
        const highlightTex = renderer.generateTexture(g);


        this.cells = [];

        // Cell placed underneath other cells to highlight mouse
        // position
        this.pointer = PIXI.Sprite.from(highlightTex);
        // Cell placed underneath other cells to highlight selection
        this.highlight = PIXI.Sprite.from(normalTex);

        for (let row = 0; row < this.rows; row++) {
            for (let col = 0; col < this.cols; col++) {
                let sprite = normalTex;
                if (themeParams.mapSprite) {
                    sprite = assets.MAP_SPRITE;
                    if (themeParams.borderSprite) {
                        if (row === 0 || col === 0 ||
                            row === this.rows - 1 || col === this.cols - 1) {
                            sprite = assets.MAP_BORDER_SPRITE;
                        }
                    }
                }
                const cell = PIXI.Sprite.from(sprite);
                cell.alpha = 0.9;
                cell.width = this.scale;
                cell.height = this.scale;
                cell.anchor.x = 0.5;
                cell.anchor.y = 0.5;
                cell.position.x = (col + 0.5) * this.scale;
                cell.position.y = (row + 0.5) * this.scale;
                const [ base, baseOpacity ] = this.productionToColor(this.productions, row, col, this.constants.MAX_CELL_PRODUCTION);
                cell.tint = alphaBlend(base, this.renderer.backgroundColor, baseOpacity);
                this.cells.push(cell);
                this.tintMap.addChild(cell);
            }
        }
    }

    get scale() {
        return this.camera.scale;
    }

    /**
     * Determines the color of a map square based on the production of said square
     * @param productions: an array containing the production value of each cell
     * @param row: the row of the cell of interest
     * @param col: the column of the cell of interest
     * Returns a color, alpha pair. Darker = more production
     */
    productionToColor(productions, row, col, MAX_PRODUCTION) {
        const production = productions[row][col];
        let production_fraction = production / MAX_PRODUCTION;
        const index = Math.floor(Math.max(0, Math.min(1, production_fraction)) * (COLOR_SCALE.length - 1));
        const result = COLOR_SCALE[index];
        return [ result, 1 ];
    }

    /**
     * Add this factory to the PIXI stage.
     * @param container {PIXI.Container} The parent container of the fish
     * sprites.
     */
    attach(container) {
        container.addChild(this.pointer);
        container.addChild(this.tintMap);
        container.addChild(this.highlight);
        if (themeParams.bloom) {
            container.filters = [new AdvancedBloomFilter({threshold: 0, brightness: 1.0})];
            container.filterArea = new PIXI.Rectangle(0, 0, this.renderer.width, this.renderer.height);
        }

        this.container = container;
    }

    /**
     * Remove the map from the visualizer.
     */
    destroy() {
        this.container.removeChild(this.tintMap);
        this.container.removeChild(this.pointer);
        this.container.removeChild(this.highlight);
        this.container = null;
    }

    /**
     * Return the color of the owner of a cell
     * @param owners: 2d array of the map, containing owner information
     *  Info can be expanded, eg for obstacles
     * @params row, col: row and column of cell of interest
     * returns a color in hex
     */
    ownerToColor(owners, row, col, _max_production) {
        const cell = owners[row][col];
        if (cell.owner < 0) {
            return [PLAYER_COLORS[0], 0];
        }
        return [PLAYER_COLORS[cell.owner], assets.OWNER_TINT_ALPHA];
    }

    /**
     * Update the fish display based on the current frame and time.
     * @param owner_grid: grid of owners of clls
     */
    update(frame, updated_cells, delta) {
        // update cell productions
        if (delta > 0) {
            if (!this.productionPatches[frame]) {
                this.productionPatches[frame] = {};
            }

            for (let cell_index = 0; cell_index < updated_cells.length; cell_index++) {
                const cell_info = updated_cells[cell_index];
                if (!this.productionPatches[frame][cell_info.y]) {
                    this.productionPatches[frame][cell_info.y] = {};
                }
                this.productionPatches[frame][cell_info.y][cell_info.x] =
                    this.productions[cell_info.y][cell_info.x];
                this.productions[cell_info.y][cell_info.x] = cell_info.production;
            }
        }
        else if (delta < 0) {
            for (const [ys, row] of Object.entries(this.productionPatches[frame + 1] || {})) {
                const y = parseInt(ys, 10);
                for (const [xs, energy] of Object.entries(row)) {
                    const x = parseInt(xs, 10);
                    this.productions[y][x] = energy;
                }
            }
        }

        // Redraw map cells, both for new production colors and possible resizing due to zooming
        for (let row = 0; row < this.rows; row++) {
            for (let col = 0; col < this.cols; col++) {
                const production = this.productions[row][col];
                const production_fraction = Math.min(1.25, production / this.constants.MAX_CELL_PRODUCTION);
                const [ base, baseOpacity ] = this.productionToColor(this.productions, row, col, this.constants.MAX_CELL_PRODUCTION);
                const cell = this.cells[row * this.cols + col];
                const bg = this.renderer.backgroundColor;
                cell.tint = alphaBlend(base, bg, baseOpacity);
                if (themeParams.scaleMapSprite) {
                    if (production_fraction <= 0.2) {
                        cell.width = Math.sqrt(production_fraction) * this.scale;
                        cell.height = Math.sqrt(production_fraction) * this.scale;
                    }
                    else {
                        cell.width = (0.3 + 0.7 * production_fraction) * this.scale;
                        cell.height = (0.3 + 0.7 * production_fraction) * this.scale;
                    }
                }
                else {
                    cell.width = this.scale;
                    cell.height = this.scale;
                }
                const [ cellX, cellY ] = this.camera.worldToCamera(col, row);
                cell.position.x = (cellX + 0.5) * this.scale;
                cell.position.y = (cellY + 0.5) * this.scale;
            }
        }
    }

    draw() {
        if (this.hovered) {
            const [ cellX, cellY ] = this.camera.worldToCamera(this.hovered.x, this.hovered.y);
            this.pointer.position.x = (cellX - 0.05) * this.scale;
            this.pointer.position.y = (cellY - 0.0) * this.scale;
            this.pointer.width = 1.1 * this.scale;
            this.pointer.height = 1.1 * this.scale;
            this.pointer.visible = true;
        }
        else {
            this.pointer.visible = false;
        }

        const camera = this.camera;
        if (camera.selected && camera.selected.type === "point") {
            const [ cellX, cellY ] = this.camera.worldToCamera(camera.selected.x, camera.selected.y);
            this.highlight.position.x = (cellX - 0.05) * this.scale;
            this.highlight.position.y = (cellY - 0.05) * this.scale;
            this.highlight.width = 1.1 * this.scale;
            this.highlight.height = 1.1 * this.scale;
            this.highlight.visible = true;
            this.highlight.alpha = 0.4;
        }
        else {
            this.highlight.visible = false;
        }
    }
}

function alphaBlend(rgb1, rgb2, alpha) {
    const b1 = rgb1 & 0xFF;
    const b2 = rgb2 & 0xFF;
    const g1 = (rgb1 & 0xFF00) >> 8;
    const g2 = (rgb2 & 0xFF00) >> 8;
    const r1 = (rgb1 & 0xFF0000) >> 16;
    const r2 = (rgb2 & 0xFF0000) >> 16;

    const r = alphaBlend1(r1, r2, alpha);
    const g = alphaBlend1(g1, g2, alpha);
    const b = alphaBlend1(b1, b2, alpha);

    return Math.floor((r << 16) | (g << 8) | b);
}

function alphaBlend1(c1, c2, alpha) {
    return Math.floor(alpha * c1 + (1 - alpha) * c2);
}
