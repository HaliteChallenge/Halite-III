import * as PIXI from "pixi.js";

import * as assets from "./assets";
import {PLAYER_COLORS} from "./assets";

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

        this.tintMap = new PIXI.particles.ParticleContainer(this.rows * this.cols, {
            vertices: true,
            position: true,
            tint: true,
        });

        this.tintMap.interactive = true;
        this.tintMap.hitArea = new PIXI.Rectangle(0, 0, renderer.width, renderer.height);
        this.tintMap.on("pointerdown", (e) => {
            const localCoords = e.data.global;
            const [ x, y ] = this.camera.scaledToScreen(localCoords.x, localCoords.y);
            const [ cellX, cellY ] = this.camera.screenToWorld(x, y);
            const production = this.productions[cellY][cellX];
            const owner = this.owners !== null ? this.owners[cellX][cellY].owner : -1;
            onSelect("point", {
                x: cellX,
                y: cellY,
                production: production,
                owner: owner,
            });
        });

        // Generate the texture for a single map cell (16x16 white
        // square with a 2 pixel 70% black border blended on top)
        // Could probably be replaced with a real texture
        const g = new PIXI.Graphics();
        const borderWidth = 1;
        const textureWidth = 16;
        g.beginFill(0xFFFFFF, 1.0);
        g.drawRect(0, 0, textureWidth, textureWidth);
        g.beginFill(0x000000, 0.7);
        // Be careful not to overlap when drawing the border, or else
        // some parts will be darker than others
        g.drawRect(0, 0, textureWidth, borderWidth);
        g.drawRect(0, borderWidth, borderWidth, textureWidth - borderWidth);
        g.drawRect(textureWidth - borderWidth, borderWidth, borderWidth, textureWidth - borderWidth);
        g.drawRect(borderWidth, textureWidth - borderWidth, textureWidth - 2*borderWidth, borderWidth);
        const tex = renderer.generateTexture(g);

        this.cells = [];

        for (let row = 0; row < this.rows; row++) {
            for (let col = 0; col < this.cols; col++) {
                const cell = PIXI.Sprite.from(tex);
                cell.width = this.scale;
                cell.height = this.scale;
                cell.position.x = col * this.scale;
                cell.position.y = row * this.scale;
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
        if (production === 0) {
            return [assets.FACTORY_BASE_COLOR, 1];
        }
        let production_fraction = production / MAX_PRODUCTION;
        if (production_fraction <= 0.3333) {
            return [alphaBlend(0x282828, assets.MAP_COLOR_LIGHT, 1-3*production_fraction), .75];
        }
        else if (production_fraction <= 0.6666) {
            return [alphaBlend(assets.MAP_COLOR_LIGHT, assets.MAP_COLOR_MEDIUM, 2-3*production_fraction), .75];
        }
        else {
            return [alphaBlend(assets.MAP_COLOR_MEDIUM, assets.MAP_COLOR_DARK, 3-3*production_fraction), .75];
        }
    }

    /**
     * Add this factory to the PIXI stage.
     * @param container {PIXI.Container} The parent container of the fish
     * sprites.
     */
    attach(container) {
        container.addChild(this.tintMap);
        this.container = container;
    }

    /**
     * Remove the map from the visualizer.
     */
    destroy() {
        this.container.removeChild(this.tintMap);
    }

    get id() {
        // TODO: do maps get IDs?
        return 0;
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
    update(updated_cells) {
        // update cell productions
        for (let cell_index = 0; cell_index < updated_cells.length; cell_index++) {
            let cell_info = updated_cells[cell_index]
            this.productions[cell_info.y][cell_info.x] = cell_info.production;
        }
        // Redraw map cells, both for new production colors and possible resizing due to zooming
        for (let row = 0; row < this.rows; row++) {
            for (let col = 0; col < this.cols; col++) {
                const [ base, baseOpacity ] = this.productionToColor(this.productions, row, col, this.constants.MAX_CELL_PRODUCTION);
                const cell = this.cells[row * this.cols + col];
                cell.tint = alphaBlend(base, this.renderer.backgroundColor, baseOpacity);
                cell.width = this.scale;
                cell.height = this.scale;
                const [ cellX, cellY ] = this.camera.worldToCamera(col, row);
                cell.position.x = cellX * this.scale;
                cell.position.y = cellY * this.scale;
            }
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
