using System;
using System.Collections.Generic;
using System.Text;
using Newtonsoft.Json;

namespace Halite3.hlt
{
    public class Constants
    {
        /// <summary>
        /// The cost to build a single ship.
        /// </summary>
        public static int SHIP_COST;

        /// <summary>
        /// The cost to build a dropoff.
        /// </summary>
        public static int DROPOFF_COST;

        /// <summary>
        /// The maximum amount of halite a ship can carry.
        /// </summary>
        public static int MAX_HALITE;

        /// <summary>
        /// The maximum number of turns a game can last. This reflects the fact
        /// that smaller maps play for fewer turns.
        /// </summary>
        public static int MAX_TURNS;

        /// <summary>
        /// 1/EXTRACT_RATIO halite (truncated) is collected from a square per turn.
        /// </summary>
        public static int EXTRACT_RATIO;

        /// <summary>
        /// 1/MOVE_COST_RATIO halite (truncated) is needed to move off a cell.
        /// </summary>
        public static int MOVE_COST_RATIO;

        /// <summary>
        /// Whether inspiration is enabled.
        /// </summary>
        public static bool INSPIRATION_ENABLED;

        /// <summary>
        /// A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent
        /// ships are within this Manhattan distance.
        /// </summary>
        public static int INSPIRATION_RADIUS;

        /// <summary>
        /// A ship is inspired if at least this many opponent ships are within
        /// INSPIRATION_RADIUS distance.
        /// </summary>
        public static int INSPIRATION_SHIP_COUNT;

        /// <summary>
        /// An inspired ship mines 1/X halite from a cell per turn instead.
        /// </summary>
        public static int INSPIRED_EXTRACT_RATIO;

        /// <summary>
        /// An inspired ship that removes Y halite from a cell collects X*Y additional halite.
        /// </summary>
        public static double INSPIRED_BONUS_MULTIPLIER;

        /// <summary>
        /// An inspired ship instead spends 1/X% halite to move.
        /// </summary>
        public static double INSPIRED_MOVE_COST_RATIO;

        /// <summary>
        /// Deserializes the JSON string of constants and stores it as a collection
        /// of static variables.
        /// </summary>
        /// <param name="constantsStr">A JSON string containing the game constants</param>
        public static void LoadConstants(string constantsStr)
        {
            Dictionary<string, string> constantsDict =
                JsonConvert.DeserializeObject<Dictionary<string, string>>(constantsStr);

            SHIP_COST = int.Parse(constantsDict["NEW_ENTITY_ENERGY_COST"]);
            DROPOFF_COST = int.Parse(constantsDict["DROPOFF_COST"]);
            MAX_HALITE = int.Parse(constantsDict["MAX_ENERGY"]);
            MAX_TURNS = int.Parse(constantsDict["MAX_TURNS"]);
            EXTRACT_RATIO = int.Parse(constantsDict["EXTRACT_RATIO"]);
            MOVE_COST_RATIO = int.Parse(constantsDict["MOVE_COST_RATIO"]);
            INSPIRATION_ENABLED = bool.Parse(constantsDict["INSPIRATION_ENABLED"]);
            INSPIRATION_RADIUS = int.Parse(constantsDict["INSPIRATION_RADIUS"]);
            INSPIRATION_SHIP_COUNT = int.Parse(constantsDict["INSPIRATION_SHIP_COUNT"]);
            INSPIRED_EXTRACT_RATIO = int.Parse(constantsDict["INSPIRED_EXTRACT_RATIO"]);
            INSPIRED_BONUS_MULTIPLIER = double.Parse(constantsDict["INSPIRED_BONUS_MULTIPLIER"]);
            INSPIRED_MOVE_COST_RATIO = int.Parse(constantsDict["INSPIRED_MOVE_COST_RATIO"]);
        }
    }
}
