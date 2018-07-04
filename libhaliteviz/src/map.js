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
                this.productions[row][col] = cell["type"] === "n" ? cell.production : 0;
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
            const [ cellX, cellY ] = this.camera.screenToWorld(localCoords.x, localCoords.y);
            const production = this.productions[cellY][cellX];
            const owner = this.owners !== null ? this.owners[cellX][cellY].owner : -1;
            onSelect("point", { x: cellX, y: cellY, production: production,
                owner: owner});
        });

        const g = new PIXI.Graphics();
        g.beginFill(0xFFFFFF, 1.0);
        g.drawRect(0, 0, 16, 16);
        g.beginFill(0x000000, 0.7);
        g.drawRect(0, 0, 16, 2);
        g.drawRect(0, 2, 2, 14);
        g.drawRect(14, 2, 2, 14);
        g.drawRect(2, 14, 12, 2);
        const tex = renderer.generateTexture(g);

        this.cells = [];

        for (let row = 0; row < this.rows; row++) {
            for (let col = 0; col < this.cols; col++) {
                const cell = PIXI.Sprite.from(tex);
                cell.width = this.scale;
                cell.height = this.scale;
                cell.position.x = col * this.scale;
                cell.position.y = row * this.scale;
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
            return [assets.FACTORY_BASE_COLOR, assets.FACTORY_BASE_ALPHA];
        }
        let production_fraction = production / MAX_PRODUCTION;
        if (production_fraction < 0.33) {
            return [assets.MAP_COLOR_LIGHT, (production_fraction / 0.33)];
        }
        else if (production_fraction < 0.66) {
            return [assets.MAP_COLOR_MEDIUM, (production_fraction - 0.33) / 0.33];
        }
        else {
            return [assets.MAP_COLOR_DARK, (production_fraction - 0.66) / 0.34];
        }
    }

    /**
     * Draw the map onto the given Graphics object
     * Can be used to draw both the base production map and any ownership tinting
     */
    drawMap(mapGraphics, rows, cols, mapArray, squareToColor, drawLines, renderer, scale, constants) {
        mapGraphics.clear();

        for (let row = 0; row < rows; row++) {
            for (let col = 0; col < cols; col++) {
                let color_arr = squareToColor(mapArray, row, col, constants.MAX_CELL_PRODUCTION);
                mapGraphics.beginFill(color_arr[0], color_arr[1]);
                mapGraphics.drawRect(col * scale, row * scale, scale, scale);
                mapGraphics.endFill();
            }
        }

        if (drawLines) {
            mapGraphics.lineStyle(assets.LINE_WIDTH, assets.LINE_COLOR);
            //Draw the map lines
            for (let row = 0; row <= rows + 2; row++) {
                // move to start of row, draw line
                // TODO: why are there 2 more columns and rows than I expect?
                mapGraphics.moveTo(0, row * scale);
                mapGraphics.lineTo((cols + 2) * scale,
                    row * scale);
            }

            for (let col = 0; col <= cols + 2; col++) {
                // move to start of col, draw line
                mapGraphics.moveTo(col * scale, 0);
                mapGraphics.lineTo(col * scale,
                    (rows + 2) * scale);
            }
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
    update(owner_grid) {
        this.owners = owner_grid;
        // Use the given grid to texture the map
        for (let row = 0; row < this.rows; row++) {
            for (let col = 0; col < this.cols; col++) {
                const [ base ] = this.productionToColor(this.productions, row, col, this.constants.MAX_CELL_PRODUCTION);
                const [ color, opacity ] = this.ownerToColor(owner_grid, row, col, this.constants.MAX_CELL_PRODUCTION);
                const cell = this.cells[row * this.cols + col];
                cell.tint = Math.floor(opacity * color + (1 - opacity) * base);
                cell.width = this.scale;
                cell.height = this.scale;
                cell.position.x = ((col + this.camera.pan.x + this.cols) % this.cols) * this.scale;
                cell.position.y = ((row + this.camera.pan.y + this.rows) % this.rows) * this.scale;
            }
        }
    }
}
