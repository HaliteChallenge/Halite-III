using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;

namespace MyBot.hlt
{
    class Constants
    {
        /** The maximum amount of halite a ship can carry. */
        public static int MAX_HALITE;
        /** The cost to build a single ship. */
        public static int SHIP_COST;
        /** The cost to build a dropoff. */
        public static int DROPOFF_COST;
        /** The maximum number of turns a game can last. */
        public static int MAX_TURNS;
        /** 1/EXTRACT_RATIO halite (rounded) is collected from a square per turn. */
        public static int EXTRACT_RATIO;
        /** 1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell. */
        public static int MOVE_COST_RATIO;
        /** Whether inspiration is enabled. */
        public static bool INSPIRATION_ENABLED;
        /** A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent ships are within this Manhattan distance. */
        public static int INSPIRATION_RADIUS;
        /** A ship is inspired if at least this many opponent ships are within INSPIRATION_RADIUS distance. */
        public static int INSPIRATION_SHIP_COUNT;
        /** An inspired ship mines 1/X halite from a cell per turn instead. */
        public static int INSPIRED_EXTRACT_RATIO;
        /** An inspired ship that removes Y halite from a cell collects X*Y additional halite. */
        public static double INSPIRED_BONUS_MULTIPLIER;
        /** An inspired ship instead spends 1/X% halite to move. */
        public static int INSPIRED_MOVE_COST_RATIO;

        public static int BOT_ID;

        public static void populateConstants( string stringFromEngine)
        {
            ConstantsSerialisable constants = (ConstantsSerialisable)JsonConvert.DeserializeObject<ConstantsSerialisable>(stringFromEngine);
            MAX_HALITE = constants.MAX_HALITE;
            SHIP_COST = constants.SHIP_COST;
            DROPOFF_COST = constants.DROPOFF_COST;
            MAX_TURNS = constants.MAX_TURNS;
            EXTRACT_RATIO = constants.EXTRACT_RATIO;
            MOVE_COST_RATIO = constants.MOVE_COST_RATIO;
            INSPIRATION_ENABLED = constants.INSPIRATION_ENABLED;
            INSPIRATION_RADIUS = constants.INSPIRATION_RADIUS;
            INSPIRATION_SHIP_COUNT = constants.INSPIRATION_SHIP_COUNT;
            INSPIRED_EXTRACT_RATIO = constants.INSPIRED_EXTRACT_RATIO;
            INSPIRED_BONUS_MULTIPLIER = constants.INSPIRED_BONUS_MULTIPLIER;
            INSPIRED_MOVE_COST_RATIO = constants.INSPIRED_MOVE_COST_RATIO;
        }

        public static void setID(int ID)
        {
            BOT_ID = ID;
        }
    }
}
