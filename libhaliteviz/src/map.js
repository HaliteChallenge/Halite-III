import * as PIXI from "pixi.js";
import * as d3ScaleChromatic from "d3-scale-chromatic";
import {AdvancedBloomFilter} from '@pixi/filter-advanced-bloom';

import * as assets from "./assets";
import {PLAYER_COLORS} from "./assets";

// Color scale modified from Colorcet linear_blue_5-95_c73
// TODO: add attribution to website
// https://github.com/bokeh/colorcet/blob/master/LICENSE.txt

const COLOR_SCALE = [
    [0.17286, 0.51122, 0.9952],
    [0.17386, 0.51627, 0.99519],
    [0.17503, 0.52131, 0.99516],
    [0.17629, 0.52633, 0.9951],
    [0.17778, 0.53132, 0.99503],
    [0.17936, 0.5363, 0.99494],
    [0.1811, 0.54125, 0.99482],
    [0.18299, 0.5462, 0.99469],
    [0.18504, 0.55111, 0.99454],
    [0.1872, 0.55601, 0.99436],
    [0.18948, 0.5609, 0.99417],
    [0.19189, 0.56577, 0.99396],
    [0.19443, 0.57063, 0.99373],
    [0.19686, 0.5755, 0.99349],
    [0.19906, 0.58036, 0.99326],
    [0.2011, 0.58524, 0.99303],
    [0.20297, 0.59011, 0.99281],
    [0.20464, 0.59499, 0.9926],
    [0.20612, 0.59989, 0.99239],
    [0.2074, 0.60479, 0.99218],
    [0.2085, 0.60969, 0.99198],
    [0.20942, 0.6146, 0.99179],
    [0.21016, 0.61953, 0.9916],
    [0.21072, 0.62445, 0.99142],
    [0.21111, 0.62938, 0.99124],
    [0.21132, 0.63432, 0.99107],
    [0.21135, 0.63926, 0.9909],
    [0.2112, 0.64421, 0.99074],
    [0.21086, 0.64918, 0.99058],
    [0.21033, 0.65414, 0.99043],
    [0.20961, 0.65911, 0.99029],
    [0.2087, 0.66409, 0.99015],
    [0.2076, 0.66908, 0.99002],
    [0.20631, 0.67406, 0.98989],
    [0.20478, 0.67906, 0.98977],
    [0.20305, 0.68406, 0.98965],
    [0.20108, 0.68908, 0.98954],
    [0.1989, 0.69409, 0.98943],
    [0.19651, 0.69911, 0.98933],
    [0.19382, 0.70414, 0.98924],
    [0.19088, 0.70918, 0.98914],
    [0.18788, 0.71421, 0.98905],
    [0.18488, 0.71923, 0.98895],
    [0.18198, 0.72423, 0.98883],
    [0.17926, 0.72923, 0.98871],
    [0.17665, 0.73421, 0.98857],
    [0.17423, 0.73918, 0.98842],
    [0.17193, 0.74412, 0.98825],
    [0.16983, 0.74906, 0.98808],
    [0.16785, 0.75399, 0.98789],
    [0.16608, 0.7589, 0.9877],
    [0.16452, 0.76381, 0.98749],
    [0.16316, 0.76869, 0.98727],
    [0.16202, 0.77357, 0.98703],
    [0.1611, 0.77844, 0.98679],
    [0.16037, 0.7833, 0.98653],
    [0.15991, 0.78814, 0.98626],
    [0.1597, 0.79298, 0.98597],
    [0.15972, 0.7978, 0.98568],
    [0.15999, 0.80261, 0.98537],
    [0.16053, 0.80742, 0.98505],
    [0.16137, 0.81221, 0.98472],
    [0.16241, 0.81699, 0.98438],
    [0.16373, 0.82176, 0.98402],
    [0.16523, 0.82652, 0.98365],
    [0.16707, 0.83127, 0.98327],
    [0.1691, 0.83601, 0.98287],
    [0.17134, 0.84074, 0.98247],
    [0.17387, 0.84546, 0.98205],
    [0.17729, 0.85015, 0.98162],
    [0.18258, 0.85474, 0.98119],
    [0.19003, 0.85923, 0.98076],
    [0.19916, 0.86363, 0.98033],
    [0.20965, 0.86794, 0.97991],
    [0.22117, 0.87217, 0.97948],
    [0.23338, 0.87632, 0.97905],
    [0.24622, 0.88041, 0.97862],
    [0.25953, 0.88443, 0.9782],
    [0.27309, 0.8884, 0.97777],
    [0.28681, 0.89231, 0.97734],
    [0.30063, 0.89617, 0.97691],
    [0.31448, 0.89998, 0.97648],
    [0.32832, 0.90375, 0.97605],
    [0.34224, 0.90747, 0.97562],
    [0.35613, 0.91115, 0.97518],
    [0.36991, 0.91479, 0.97475],
    [0.38361, 0.9184, 0.97431],
    [0.39728, 0.92197, 0.97387],
    [0.4109, 0.9255, 0.97343],
    [0.42439, 0.92899, 0.973],
    [0.43779, 0.93247, 0.97255],
    [0.45116, 0.9359, 0.97211],
    [0.46443, 0.93931, 0.97166],
    [0.47755, 0.94269, 0.97121],
    [0.49069, 0.94603, 0.97076],
    [0.5037, 0.94935, 0.97031],
    [0.51659, 0.95265, 0.96986],
    [0.52949, 0.95592, 0.9694],
    [0.54227, 0.95916, 0.96894],
    [0.55496, 0.96238, 0.96849],
    [0.56763, 0.96557, 0.96803],
    [0.58018, 0.96874, 0.96756],
    [0.59271, 0.97189, 0.9671],
    [0.60515, 0.97501, 0.96663],
    [0.61751, 0.97812, 0.96616],
    [0.62987, 0.98119, 0.96569],
    [0.64213, 0.98425, 0.96521],
    [0.65436, 0.98729, 0.96474],
    [0.66654, 0.9903, 0.96426],
    [0.67864, 0.9933, 0.96378],
    [0.69072, 0.99626, 0.9633],
    [0.70271, 0.99922, 0.96282]
];

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
        const borderWidth = 0;//1; TWEAK -- remove board
        const textureWidth = 16;
        let g = new PIXI.Graphics();

        // TWEAK: circular cells
        // g.beginFill(0xFFFFFF, 1.0);
        // g.drawCircle(8, 8, 8);

        // TWEAK: square cells
        g.beginFill(0xFFFFFF, 1.0);
        g.drawRect(0, 0, textureWidth, textureWidth);
        // Be careful not to overlap when drawing the border, or else
        // some parts will be darker than others
        g.beginFill(0x000000, 0.5);
        g.drawRect(0, 0, textureWidth, borderWidth);
        g.drawRect(0, borderWidth, borderWidth, textureWidth - borderWidth);
        g.drawRect(textureWidth - borderWidth, borderWidth, borderWidth, textureWidth - borderWidth);
        g.drawRect(borderWidth, textureWidth - borderWidth, textureWidth - 2*borderWidth, borderWidth);
        // END TWEAK: square cells
        const normalTex = renderer.generateTexture(g);

        this.cells = [];

        // Cell placed underneath other cells to highlight mouse
        // position
        this.pointer = PIXI.Sprite.from(normalTex);
        // Cell placed underneath other cells to highlight selection
        this.highlight = PIXI.Sprite.from(normalTex);

        for (let row = 0; row < this.rows; row++) {
            for (let col = 0; col < this.cols; col++) {
                const cell = PIXI.Sprite.from(normalTex);
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

        // TWEAK: this uses a light blue color scale
        const index = Math.floor(Math.max(0, Math.min(1, production_fraction)) * (COLOR_SCALE.length - 1));
        const [r,g,b] = COLOR_SCALE[index];
        let result = 0;
        result += Math.floor(r * 255) << 16;
        result += Math.floor(g * 255) << 8;
        result += Math.floor(b * 255);

        // TWEAK: choose one of the two lines below, and the lines
        // under it, to choose different color scales. More scales at
        // https://github.com/d3/d3-scale-chromatic

        // const rgbString = d3ScaleChromatic.interpolateCool(production_fraction);
        // const rgbString = d3ScaleChromatic.interpolateBlues(production_fraction);
        // const parts = rgbString.slice(4, -1).split(", ");
        // let result = 0;
        // result += parseInt(parts[0], 10) << 16;
        // result += parseInt(parts[1], 10) << 8;
        // result += parseInt(parts[2], 10);
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
        container.filters = [new AdvancedBloomFilter({threshold: 0, brightness: 1.0})];
        container.filterArea = new PIXI.Rectangle(0, 0, this.renderer.width, this.renderer.height);

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
                const production_fraction = production / this.constants.MAX_CELL_PRODUCTION;
                const [ base, baseOpacity ] = this.productionToColor(this.productions, row, col, this.constants.MAX_CELL_PRODUCTION);
                const cell = this.cells[row * this.cols + col];
                const bg = this.renderer.backgroundColor;
                cell.tint = alphaBlend(base, bg, baseOpacity);
                // TWEAK: uncomment to make size vary with amount of halite
                if (production_fraction == 0 ) {
                    cell.width = 0
                    cell.height = 0
                }
                else {
                    cell.width = (0.2 + 0.8 * production_fraction) * this.scale;
                    cell.height = (0.2 + 0.8 * production_fraction) * this.scale;    
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
            this.pointer.position.x = cellX * this.scale;
            this.pointer.position.y = cellY * this.scale;
            this.pointer.width = this.scale;
            this.pointer.height = this.scale;
            this.pointer.visible = true;
        }
        else {
            this.pointer.visible = false;
        }

        const camera = this.camera;
        if (camera.selected && camera.selected.type === "point") {
            const [ cellX, cellY ] = this.camera.worldToCamera(camera.selected.x, camera.selected.y);
            this.highlight.position.x = cellX * this.scale;
            this.highlight.position.y = cellY * this.scale;
            this.highlight.width = this.scale;
            this.highlight.height = this.scale;
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
