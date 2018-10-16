<?php

namespace hlt;

class Constants
{
    /**
     * The maximum amount of halite a ship can carry.
     * @var int
     */
    public static $MAX_HALITE;

    /**
     * The cost to build a single ship.
     * @var int
     */
    public static $SHIP_COST;

    /**
     * The cost to build a dropoff.
     * @var int
     */
    public static $DROPOFF_COST;

    /**
     * The maximum number of turns a game can last.
     * @var int
     */
    public static $MAX_TURNS;

    /**
     * 1/EXTRACT_RATIO halite (rounded) is collected from a square per turn.
     * @var int
     */
    public static $EXTRACT_RATIO;

    /**
     * 1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell.
     * @var int
     */
    public static $MOVE_COST_RATIO;

    /**
     * Whether inspiration is enabled.
     * @var boolean
     */
    public static $INSPIRATION_ENABLED;

    /**
     * A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent ships are within this Manhattan distance.
     * @var int
     */
    public static $INSPIRATION_RADIUS;

    /**
     * A ship is inspired if at least this many opponent ships are within INSPIRATION_RADIUS distance.
     * @var int
     */
    public static $INSPIRATION_SHIP_COUNT;

    /**
     * An inspired ship mines 1/X halite from a cell per turn instead.
     * @var int
     */
    public static $INSPIRED_EXTRACT_RATIO;

    /**
     * An inspired ship that removes Y halite from a cell collects X*Y additional halite.
     * @var double
     */
    public static $INSPIRED_BONUS_MULTIPLIER;

    /**
     * An inspired ship instead spends 1/X% halite to move.
     * @var int
     */
    public static $INSPIRED_MOVE_COST_RATIO;

    public static function populateConstants(string $stringFromEngine): void
    {
        $constants = json_decode($stringFromEngine, true);

        self::$SHIP_COST = $constants['NEW_ENTITY_ENERGY_COST'];
        self::$DROPOFF_COST = $constants['DROPOFF_COST'];
        self::$MAX_HALITE = $constants['MAX_ENERGY'];
        self::$MAX_TURNS = $constants['MAX_TURNS'];
        self::$EXTRACT_RATIO = $constants['EXTRACT_RATIO'];
        self::$MOVE_COST_RATIO = $constants['MOVE_COST_RATIO'];
        self::$INSPIRATION_ENABLED = $constants['INSPIRATION_ENABLED'];
        self::$INSPIRATION_RADIUS = $constants['INSPIRATION_RADIUS'];
        self::$INSPIRATION_SHIP_COUNT = $constants['INSPIRATION_SHIP_COUNT'];
        self::$INSPIRED_EXTRACT_RATIO = $constants['INSPIRED_EXTRACT_RATIO'];
        self::$INSPIRED_BONUS_MULTIPLIER = $constants['INSPIRED_BONUS_MULTIPLIER'];
        self::$INSPIRED_MOVE_COST_RATIO = $constants['INSPIRED_MOVE_COST_RATIO'];
    }
}
