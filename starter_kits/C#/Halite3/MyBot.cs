using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Halite_Project.hlt;

namespace Halite_Project {
    class MyBot {
        public static void Main(string[] args) {
            long rngSeed;
            if(args.Length > 1) {
                rngSeed = int.Parse(args[1]);
            } else {
                rngSeed = Stopwatch.GetTimestamp();
            }
            Random rng = new Random((int) rngSeed);

            Game game = new Game();
            // At this point "game" variable is populated with initial map data.
            // This is a good place to do computationally expensive start-up pre-processing.
            // As soon as you call "ready" function below, the 2 second per turn timer will start.
            game.Ready("MyC#Bot");

            Log.CreateLog("Successfully created bot! My Player ID is " + game.myId + ". Bot rng seed is " + rngSeed + ".");
            
            while(game.turnNumber < Constants.MAX_TURNS) {
                game.UpdateFrame();
                Player me = game.me;
                GameMap gameMap = game.gameMap;

                List<Command> commandQueue = new List<Command>();

                foreach(Ship ship in me.ships.Values) {
                    if(gameMap.At(ship).halite < Constants.MAX_HALITE / 10 || ship.IsFull()) {
                        EDirection randomDirection = Direction.ALL_CARDINALS[rng.Next(4)];
                        commandQueue.Add(ship.Move(randomDirection));
                    } else {
                        commandQueue.Add(ship.StayStill());
                    }
                }

                if (game.turnNumber <= 200 && me.halite >= Constants.SHIP_COST && !gameMap.At(me.shipyard).IsOccupied()) {
                    commandQueue.Add(me.shipyard.Spawn()); //spawn a new ship
                }

                game.EndTurn(commandQueue);
            }
        }
    }
}
