/**
 * Global constants relating to the game engine.
 */

module.exports = {
    /**
     * The maximum amount of halite a ship can carry.
     */
    MAX_HALITE: 1000,

    /**
     * The cost to build a single ship.
     */
    SHIP_COST: 500,

    /**
     * The cost to build a dropoff.
     */
    DROPOFF_COST: 2000,

    /**
     * The maximum number of turns a game can last.
     */
    MAX_TURNS: 500,

    /**
     * 1/EXTRACT_RATIO halite (rounded) is collected from a square per turn.
     */
    EXTRACT_RATIO: 4,

    /**
     * 1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell.
     */
    MOVE_COST_RATIO: 10,

    /**
     * Whether the engine will terminate bots for nonsensical commands.
     */
    STRICT_ERRORS: false,

    /**
     * Load constants from JSON given by the game engine.
     */
    loadConstants: function loadConstants(constants) {
        this.SHIP_COST = typeof constants.NEW_ENTITY_ENERGY_COST !== "undefined" ?
            constants.NEW_ENTITY_ENERGY_COST : this.SHIP_COST;
        this.DROPOFF_COST = typeof constants.DROPOFF_COST !== "undefined" ?
            constants.DROPOFF_COST : this.DROPOFF_COST;
        this.MAX_HALITE = typeof constants.MAX_ENERGY !== "undefined" ?
            constants.MAX_ENERGY : this.MAX_HALITE;
        this.MAX_TURNS = typeof constants.MAX_TURNS !== "undefined" ?
            constants.MAX_TURNS : this.MAX_TURNS;
        this.EXTRACT_RATIO = typeof constants.EXTRACT_RATIO !== "undefined" ?
            constants.EXTRACT_RATIO : this.EXTRACT_RATIO;
        this.MOVE_COST_RATIO = typeof constants.MOVE_COST_RATIO !== "undefined" ?
            constants.MOVE_COST_RATIO : this.MOVE_COST_RATIO;
        this.STRICT_ERRORS = typeof constants.STRICT_ERRORS !== "undefined" ?
            constants.STRICT_ERRORS : this.STRICT_ERRORS;
    },
};
