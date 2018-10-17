using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Halite_Project.hlt {
    class Constants {
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

        public static void PopulateConstants(string stringFromEngine) {
            string[] rawTokens = Regex.Split(stringFromEngine, "[{}, :\"]+");// stringFromEngine.Split(new string[] { "[{}, :\"]+" }, StringSplitOptions.None);
            List<string> tokens = new List<string>();

            for(int i = 0; i < rawTokens.Length; i++) {
                if(rawTokens[i].Length != 0) {
                    tokens.Add(rawTokens[i]);
                }
            }

            if((tokens.Count % 2) != 0) {
                Log.CreateLog("Error: constants: expected even total number of key and value tokens from server.");
                throw new Exception();
            }

            Dictionary<string, string> constantsMap = new Dictionary<string, string>();

            for(int i = 0; i < tokens.Count; i += 2) {
                constantsMap.Add(tokens[i], tokens[i + 1]);
            }

            SHIP_COST = GetInt(constantsMap, "NEW_ENTITY_ENERGY_COST");
            DROPOFF_COST = GetInt(constantsMap, "DROPOFF_COST");
            MAX_HALITE = GetInt(constantsMap, "MAX_ENERGY");
            MAX_TURNS = GetInt(constantsMap, "MAX_TURNS");
            EXTRACT_RATIO = GetInt(constantsMap, "EXTRACT_RATIO");
            MOVE_COST_RATIO = GetInt(constantsMap, "MOVE_COST_RATIO");
            INSPIRATION_ENABLED = GetBool(constantsMap, "INSPIRATION_ENABLED");
            INSPIRATION_RADIUS = GetInt(constantsMap, "INSPIRATION_RADIUS");
            INSPIRATION_SHIP_COUNT = GetInt(constantsMap, "INSPIRATION_SHIP_COUNT");
            INSPIRED_EXTRACT_RATIO = GetInt(constantsMap, "INSPIRED_EXTRACT_RATIO");
            INSPIRED_BONUS_MULTIPLIER = GetDouble(constantsMap, "INSPIRED_BONUS_MULTIPLIER");
            INSPIRED_MOVE_COST_RATIO = GetInt(constantsMap, "INSPIRED_MOVE_COST_RATIO");
        }

        private static int GetInt(Dictionary<string, string> map, string key) {
            return int.Parse(GetString(map, key));
        }

        private static double GetDouble(Dictionary<string, string> map, string key) {
            return double.Parse(GetString(map, key));
        }

        private static bool GetBool(Dictionary<string, string> map, string key) {
            string stringValue = GetString(map, key);

            switch (stringValue) {
                case "true": return true;
                case "false": return false;
                default:
                    Log.CreateLog("Error: constants: " + key + " constant has value of '" + stringValue +
                    "' from server. Do not know how to parse that as boolean.");
                    throw new Exception();
            }
        }

        private static string GetString(Dictionary<string, string> map, string key) {
            if (!map.ContainsKey(key)) {
                Log.CreateLog("Error: constants: server did not send " + key + " constant.");
                throw new Exception();
            }

            return map[key];
        }
    }
}