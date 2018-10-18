
/** The maximum amount of halite a ship can carry. */
int MAX_HALITE;
/** The cost to build a single ship. */
int SHIP_COST;
/** The cost to build a dropoff. */
int DROPOFF_COST;
/** The maximum number of turns a game can last. */
int MAX_TURNS;
/** 1/EXTRACT_RATIO halite (rounded) is collected from a square per turn. */
int EXTRACT_RATIO;
/** 1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell. */
int MOVE_COST_RATIO;
/** Whether inspiration is enabled. */
bool INSPIRATION_ENABLED;
/** A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent ships are within this Manhattan distance. */
int INSPIRATION_RADIUS;
/** A ship is inspired if at least this many opponent ships are within INSPIRATION_RADIUS distance. */
int INSPIRATION_SHIP_COUNT;
/** An inspired ship mines 1/X halite from a cell per turn instead. */
int INSPIRED_EXTRACT_RATIO;
/** An inspired ship that removes Y halite from a cell collects X*Y additional halite. */
double INSPIRED_BONUS_MULTIPLIER;
/** An inspired ship instead spends 1/X% halite to move. */
int INSPIRED_MOVE_COST_RATIO;
