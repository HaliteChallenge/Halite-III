using System;
using System.Collections.Generic;
using System.Linq;

namespace Halite3.Core
{
    public class Constants
    {
        /** The maximum amount of halite a ship can carry. */
        public static int MAX_HALITE { get; private set; }
        /** The cost to build a single ship. */
        public static int SHIP_COST { get; private set; }
        /** The cost to build a dropoff. */
        public static int DROPOFF_COST { get; private set; }
        /** The maximum number of turns a game can last. */
        public static int MAX_TURNS { get; private set; }
        /** 1/EXTRACT_RATIO halite (rounded) is collected from a square per turn. */
        public static int EXTRACT_RATIO { get; private set; }
        /** 1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell. */
        public static int MOVE_COST_RATIO { get; private set; }
        /** Whether inspiration is enabled. */
        public static bool INSPIRATION_ENABLED { get; private set; }
        /** A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent ships are within this Manhattan distance. */
        public static int INSPIRATION_RADIUS { get; private set; }
        /** A ship is inspired if at least this many opponent ships are within INSPIRATION_RADIUS distance. */
        public static int INSPIRATION_SHIP_COUNT { get; private set; }
        /** An inspired ship mines 1/X halite from a cell per turn instead. */
        public static int INSPIRED_EXTRACT_RATIO { get; private set; }
        /** An inspired ship that removes Y halite from a cell collects X*Y additional halite. */
        public static double INSPIRED_BONUS_MULTIPLIER { get; private set; }
        /** An inspired ship instead spends 1/X% halite to move. */
        public static int INSPIRED_MOVE_COST_RATIO { get; private set; }

        public static void populateConstants(string stringFromEngine)
        {
            var temp = stringFromEngine.Replace('{', ' ');
            temp = temp.Replace('}', ' ');
            temp = temp.Replace('"', ' ');

            var tokens = temp.Split(',');

            var lookup = tokens.Select(token => {
                var split = token.Split(':');
                return new { key = split[0].Trim(), value = split[1].Trim() };
            }).ToDictionary(token => token.key, token => token.value);

            var constantsMap = new Dictionary<string, string>();

            SHIP_COST = int.Parse(lookup["NEW_ENTITY_ENERGY_COST"]);
            DROPOFF_COST = int.Parse(lookup["DROPOFF_COST"]);
            MAX_HALITE = int.Parse(lookup["MAX_ENERGY"]);
            MAX_TURNS = int.Parse(lookup["MAX_TURNS"]);
            EXTRACT_RATIO = int.Parse(lookup["EXTRACT_RATIO"]);
            MOVE_COST_RATIO = int.Parse(lookup["MOVE_COST_RATIO"]);
            INSPIRATION_ENABLED = bool.Parse(lookup["INSPIRATION_ENABLED"]);
            INSPIRATION_RADIUS = int.Parse(lookup["INSPIRATION_RADIUS"]);
            INSPIRATION_SHIP_COUNT = int.Parse(lookup["INSPIRATION_SHIP_COUNT"]);
            INSPIRED_EXTRACT_RATIO = int.Parse(lookup["INSPIRED_EXTRACT_RATIO"]);
            INSPIRED_BONUS_MULTIPLIER = double.Parse(lookup["INSPIRED_BONUS_MULTIPLIER"]);
            INSPIRED_MOVE_COST_RATIO = int.Parse(lookup["INSPIRED_MOVE_COST_RATIO"]);
        }
    }
}
