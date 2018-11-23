/**
 * Global constants relating to the game engine.
 */

export class Constants {
    /**
     * The maximum number of map cells to consider in navigation.
     */
    public static readonly MAX_BFS_STEPS = 1024;  // search an entire 32x32 region

    public static SHIP_COST = 0;
    public static DROPOFF_COST = 0;
    public static MAX_ENERGY = 0;
    public static MAX_TURNS = 0;
    public static EXTRACT_RATIO = 0;
    public static MOVE_COST_RATIO = 0;
    public static INSPIRATION_ENABLED = 0;
    public static INSPIRATION_RADIUS = 0;
    public static INSPIRATION_SHIP_COUNT = 0;
    public static INSPIRED_EXTRACT_RATIO = 0;
    public static INSPIRED_BONUS_MULTIPLIER = 0;
    public static INSPIRED_MOVE_COST_RATIO = 0;

    /**
     * Load constants from JSON given by the game engine.
     */
    public static loadConstants(aConstants: any) {
        /** The cost to build a single ship. */
        this.SHIP_COST = aConstants.NEW_ENTITY_ENERGY_COST;

        /** The cost to build a dropoff. */
        this.DROPOFF_COST = aConstants.DROPOFF_COST;

        /** The maximum amount of halite a ship can carry. */
        this.MAX_ENERGY = aConstants.MAX_ENERGY;  // FIXME: check name

        /**
         * The maximum number of turns a game can last. This reflects
         * the fact that smaller maps play for fewer turns.
         */
        this.MAX_TURNS = aConstants.MAX_TURNS;

        /** 1/EXTRACT_RATIO halite (truncated) is collected from a square per turn. */
        this.EXTRACT_RATIO = aConstants.EXTRACT_RATIO;

        /** 1/MOVE_COST_RATIO halite (truncated) is needed to move off a cell. */
        this.MOVE_COST_RATIO = aConstants.MOVE_COST_RATIO;

        /** Whether inspiration is enabled. */
        this.INSPIRATION_ENABLED = aConstants.INSPIRATION_ENABLED;

        /**
         * A ship is inspired if at least INSPIRATION_SHIP_COUNT
         * opponent ships are within this Manhattan distance.
         */
        this.INSPIRATION_RADIUS = aConstants.INSPIRATION_RADIUS;

        /**
         * A ship is inspired if at least this many opponent ships are
         * within INSPIRATION_RADIUS distance.
         */
        this.INSPIRATION_SHIP_COUNT = aConstants.INSPIRATION_SHIP_COUNT;

        /** An inspired ship mines 1/X halite from a cell per turn instead. */
        this.INSPIRED_EXTRACT_RATIO = aConstants.INSPIRED_EXTRACT_RATIO;

        /** An inspired ship that removes Y halite from a cell collects X*Y additional halite. */
        this.INSPIRED_BONUS_MULTIPLIER = aConstants.INSPIRED_BONUS_MULTIPLIER;

        /** An inspired ship instead spends 1/X% halite to move. */
        this.INSPIRED_MOVE_COST_RATIO = aConstants.INSPIRED_MOVE_COST_RATIO;
    }
}
