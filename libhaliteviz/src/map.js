import * as PIXI from "pixi.js";
import * as d3ScaleChromatic from "d3-scale-chromatic";
import {AdvancedBloomFilter} from '@pixi/filter-advanced-bloom';

import * as assets from "./assets";
import {PLAYER_COLORS} from "./assets";

const COLOR_SCALE = [
    [0, 0.047803, 0.4883],
    [0, 0.049756, 0.49556],
    [0, 0.051605, 0.50281],
    [0, 0.053545, 0.51004],
    [0, 0.055585, 0.51721],
    [0, 0.057566, 0.52435],
    [0, 0.05978, 0.53144],
    [0, 0.061812, 0.53849],
    [0, 0.064016, 0.5455],
    [0, 0.066232, 0.55245],
    [0, 0.068551, 0.55934],
    [0, 0.070824, 0.5662],
    [0, 0.073229, 0.57299],
    [0, 0.07557, 0.57971],
    [0, 0.078003, 0.58638],
    [0, 0.080554, 0.59299],
    [0, 0.083114, 0.59951],
    [0, 0.085697, 0.60596],
    [0, 0.08842, 0.61236],
    [0, 0.091168, 0.61866],
    [0, 0.093925, 0.62487],
    [0, 0.096707, 0.63101],
    [0, 0.09963, 0.63705],
    [0, 0.1026, 0.64298],
    [0, 0.10561, 0.64881],
    [0, 0.10866, 0.65454],
    [0, 0.11183, 0.66016],
    [0, 0.11497, 0.66567],
    [0, 0.11829, 0.67103],
    [0, 0.12156, 0.67626],
    [0, 0.12498, 0.68134],
    [0, 0.12846, 0.68629],
    [0, 0.13201, 0.69107],
    [0, 0.13559, 0.6957],
    [0, 0.13927, 0.70014],
    [0, 0.14307, 0.70439],
    [0, 0.1469, 0.70845],
    [0, 0.15085, 0.71227],
    [0, 0.15487, 0.71588],
    [0, 0.159, 0.71923],
    [0, 0.16323, 0.7223],
    [0, 0.16754, 0.7251],
    [0, 0.17195, 0.72757],
    [0, 0.17647, 0.72972],
    [0, 0.18113, 0.73149],
    [0, 0.18594, 0.73289],
    [0, 0.19074, 0.73398],
    [0, 0.19556, 0.73486],
    [0, 0.20033, 0.73556],
    [0, 0.20512, 0.73608],
    [0, 0.20987, 0.73643],
    [0, 0.21461, 0.73659],
    [0, 0.21934, 0.73657],
    [0, 0.22402, 0.73637],
    [0, 0.22875, 0.73599],
    [0, 0.2334, 0.73544],
    [0, 0.23809, 0.73469],
    [0, 0.24275, 0.73376],
    [0, 0.24743, 0.73266],
    [0, 0.25208, 0.73137],
    [0, 0.25673, 0.72991],
    [0, 0.26137, 0.72825],
    [0, 0.26603, 0.72642],
    [0, 0.27068, 0.72441],
    [0, 0.27531, 0.72221],
    [0, 0.27995, 0.71983],
    [0, 0.28458, 0.71727],
    [0, 0.28924, 0.71452],
    [0, 0.29387, 0.71161],
    [0, 0.29852, 0.70851],
    [0, 0.30317, 0.70521],
    [0, 0.30782, 0.70174],
    [0, 0.31248, 0.69809],
    [0, 0.31716, 0.69426],
    [0, 0.32182, 0.69025],
    [0, 0.32649, 0.68607],
    [0, 0.33116, 0.68178],
    [0, 0.33582, 0.67746],
    [0, 0.34046, 0.6731],
    [0, 0.34509, 0.66871],
    [0, 0.3497, 0.66429],
    [0, 0.3543, 0.65984],
    [0, 0.35888, 0.65536],
    [0, 0.36346, 0.65085],
    [0, 0.36803, 0.6463],
    [0, 0.37258, 0.64173],
    [0, 0.37713, 0.63713],
    [0, 0.38167, 0.6325],
    [0, 0.38618, 0.62783],
    [0, 0.39071, 0.62313],
    [0, 0.39523, 0.6184],
    [0, 0.39972, 0.61363],
    [0, 0.40423, 0.60885],
    [0, 0.40872, 0.60402],
    [0, 0.41321, 0.59915],
    [0, 0.41769, 0.59426],
    [0, 0.42215, 0.58932],
    [0, 0.42663, 0.58437],
    [0, 0.4311, 0.57937],
    [0, 0.43556, 0.57433],
    [0, 0.44001, 0.56926],
    [0, 0.44446, 0.56416],
    [0, 0.44891, 0.55902],
    [0, 0.45334, 0.55384],
    [0, 0.45778, 0.54863],
    [0, 0.46222, 0.54336],
    [0, 0.46665, 0.53802],
    [0, 0.47105, 0.53253],
    [0, 0.47545, 0.52691],
    [0, 0.47982, 0.52115],
    [0, 0.48417, 0.51525],
    [0, 0.48852, 0.50921],
    [0, 0.49284, 0.50301],
    [0, 0.49717, 0.49668],
    [0, 0.50147, 0.49022],
    [0, 0.50575, 0.48359],
    [0, 0.51003, 0.47682],
    [0, 0.51431, 0.4699],
    [0, 0.51856, 0.4628],
    [0.0097866, 0.52281, 0.45558],
    [0.023896, 0.52704, 0.44818],
    [0.038383, 0.53126, 0.44061],
    [0.051763, 0.53547, 0.43289],
    [0.063442, 0.53968, 0.42499],
    [0.073828, 0.54388, 0.41692],
    [0.083244, 0.54807, 0.40866],
    [0.092062, 0.55225, 0.40022],
    [0.10019, 0.55642, 0.39159],
    [0.10786, 0.56059, 0.38276],
    [0.11513, 0.56474, 0.37372],
    [0.12206, 0.56889, 0.36445],
    [0.12871, 0.57304, 0.35498],
    [0.13507, 0.57718, 0.34524],
    [0.14115, 0.58131, 0.33527],
    [0.14697, 0.58544, 0.32499],
    [0.15257, 0.58954, 0.31449],
    [0.15773, 0.59367, 0.30393],
    [0.16231, 0.59779, 0.29352],
    [0.16631, 0.60191, 0.28332],
    [0.16984, 0.60603, 0.27332],
    [0.17292, 0.61015, 0.26352],
    [0.17565, 0.61427, 0.25387],
    [0.17811, 0.6184, 0.24439],
    [0.18021, 0.62252, 0.23514],
    [0.18207, 0.62664, 0.22606],
    [0.18374, 0.63076, 0.21715],
    [0.18522, 0.63487, 0.2084],
    [0.18649, 0.63898, 0.19982],
    [0.18765, 0.64309, 0.19148],
    [0.18863, 0.6472, 0.18334],
    [0.18951, 0.6513, 0.1754],
    [0.19029, 0.65539, 0.16767],
    [0.19099, 0.65948, 0.16013],
    [0.19162, 0.66357, 0.15293],
    [0.19219, 0.66765, 0.14604],
    [0.19272, 0.67172, 0.13937],
    [0.1932, 0.67579, 0.13311],
    [0.19366, 0.67985, 0.1272],
    [0.1941, 0.68391, 0.1218],
    [0.19451, 0.68797, 0.11692],
    [0.1949, 0.69202, 0.11259],
    [0.19529, 0.69606, 0.10881],
    [0.19569, 0.70009, 0.10581],
    [0.19611, 0.70412, 0.10358],
    [0.19653, 0.70814, 0.10209],
    [0.19694, 0.71215, 0.10139],
    [0.19736, 0.71617, 0.10116],
    [0.19779, 0.72018, 0.10101],
    [0.19823, 0.7242, 0.10087],
    [0.19868, 0.72822, 0.10073],
    [0.19914, 0.73225, 0.1006],
    [0.19961, 0.73627, 0.10048],
    [0.20009, 0.74031, 0.10036],
    [0.20058, 0.74434, 0.10025],
    [0.20108, 0.74838, 0.10015],
    [0.20159, 0.75242, 0.10006],
    [0.20211, 0.75647, 0.099977],
    [0.20265, 0.76052, 0.099902],
    [0.2032, 0.76457, 0.099835],
    [0.20376, 0.76862, 0.099777],
    [0.20433, 0.77267, 0.099729],
    [0.20488, 0.77674, 0.099691],
    [0.20546, 0.7808, 0.099663],
    [0.20608, 0.78487, 0.099645],
    [0.20669, 0.78894, 0.099637],
    [0.20729, 0.79301, 0.099641],
    [0.20791, 0.79708, 0.099656],
    [0.20855, 0.80116, 0.099683],
    [0.2092, 0.80523, 0.09972],
    [0.20987, 0.80932, 0.09977],
    [0.21055, 0.8134, 0.099833],
    [0.21125, 0.81749, 0.099908],
    [0.21196, 0.82159, 0.099996],
    [0.21266, 0.82567, 0.1001],
    [0.2134, 0.82977, 0.10021],
    [0.21454, 0.83386, 0.10034],
    [0.21746, 0.83784, 0.10049],
    [0.22334, 0.84166, 0.10065],
    [0.23182, 0.84531, 0.10083],
    [0.24228, 0.84884, 0.10102],
    [0.25428, 0.85224, 0.10123],
    [0.26735, 0.85553, 0.10145],
    [0.28125, 0.85872, 0.10168],
    [0.29571, 0.86184, 0.10193],
    [0.31067, 0.86485, 0.10219],
    [0.32594, 0.86779, 0.10246],
    [0.34137, 0.87065, 0.10275],
    [0.35684, 0.87346, 0.10306],
    [0.3724, 0.87619, 0.10337],
    [0.38805, 0.87886, 0.10369],
    [0.40366, 0.88147, 0.10401],
    [0.41921, 0.88402, 0.10433],
    [0.43478, 0.88651, 0.10468],
    [0.45028, 0.88894, 0.10507],
    [0.4657, 0.89133, 0.10549],
    [0.48111, 0.89365, 0.10592],
    [0.49641, 0.89593, 0.10637],
    [0.51169, 0.89815, 0.10681],
    [0.52691, 0.90032, 0.10725],
    [0.54202, 0.90245, 0.1077],
    [0.55713, 0.90452, 0.10817],
    [0.57212, 0.90655, 0.10869],
    [0.5871, 0.90852, 0.10927],
    [0.60201, 0.91045, 0.10983],
    [0.61689, 0.91232, 0.11037],
    [0.63169, 0.91415, 0.11095],
    [0.64646, 0.91593, 0.11155],
    [0.66118, 0.91767, 0.11216],
    [0.67585, 0.91934, 0.11281],
    [0.69049, 0.92098, 0.11347],
    [0.70508, 0.92257, 0.11409],
    [0.71966, 0.92411, 0.11477],
    [0.73418, 0.9256, 0.11552],
    [0.74868, 0.92704, 0.11627],
    [0.76314, 0.92844, 0.11699],
    [0.77759, 0.92979, 0.11777],
    [0.79201, 0.93109, 0.11859],
    [0.8064, 0.93233, 0.11937],
    [0.82077, 0.93353, 0.12019],
    [0.83511, 0.93468, 0.12102],
    [0.84946, 0.93578, 0.12189],
    [0.86375, 0.93684, 0.12278],
    [0.87808, 0.93783, 0.1237],
    [0.89234, 0.93878, 0.12464],
    [0.90664, 0.93968, 0.12562],
    [0.92088, 0.94052, 0.12657],
    [0.93514, 0.94131, 0.12755],
    [0.94939, 0.94206, 0.12857],
    [0.9636, 0.94275, 0.12961],
    [0.97785, 0.94338, 0.13068],
    [0.99205, 0.94397, 0.13172],
    [1, 0.94449, 0.13281],
    [1, 0.94497, 0.13392],
    [1, 0.94539, 0.13505],
    [1, 0.94574, 0.13614],
    [1, 0.94606, 0.13735],
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
        const borderWidth = 1;
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

        // TWEAK: this uses a deep blue-yellow color scale
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
                // cell.width = (0.5 + 0.5 * production_fraction) * this.scale;
                // cell.height = (0.5 + 0.5 * production_fraction) * this.scale;
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
