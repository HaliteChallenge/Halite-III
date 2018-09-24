#pragma once

#include <string>

namespace hlt {
    /**
     * The constants representing the game variation being played.
     * They come from game engine and changing them has no effect.
     * They are strictly informational.
     */
    namespace constants {
        void populate_constants(const std::string& string_from_engine);

        /** The maximum amount of halite a ship can carry. */
        extern int MAX_HALITE;
        /** The cost to build a single ship. */
        extern int SHIP_COST;
        /** The cost to build a dropoff. */
        extern int DROPOFF_COST;
        /** The maximum number of turns a game can last. */
        extern int MAX_TURNS;
        /** 1/EXTRACT_RATIO halite (rounded) is collected from a square per turn. */
        extern int EXTRACT_RATIO;
        /** 1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell. */
        extern int MOVE_COST_RATIO;
        /** Whether inspiration is enabled. */
        extern bool INSPIRATION_ENABLED;
        /** A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent ships are within this Manhattan distance. */
        extern int INSPIRATION_RADIUS;
        /** A ship is inspired if at least this many opponent ships are within INSPIRATION_RADIUS distance. */
        extern int INSPIRATION_SHIP_COUNT;
        /** An inspired ship mines 1/X halite from a cell per turn instead. */
        extern int INSPIRED_EXTRACT_RATIO;
        /** An inspired ship that removes Y halite from a cell collects X*Y additional halite. */
        extern double INSPIRED_BONUS_MULTIPLIER;
        /** An inspired ship instead spends 1/X% halite to move. */
        extern int INSPIRED_MOVE_COST_RATIO;
    }
}
