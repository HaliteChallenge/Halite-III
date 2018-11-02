using Halite3.hlt;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Halite3
{
    public class MyBot
    {
        public static void Main(string[] args)
        {
            int rngSeed;
            if (args.Length > 1)
            {
                rngSeed = int.Parse(args[1]);
            }
            else
            {
                rngSeed = System.DateTime.Now.Millisecond;
            }
            Random rng = new Random(rngSeed);

            Game game = new Game();
            // At this point "game" variable is populated with initial map data.
            // This is a good place to do computationally expensive start-up pre-processing.
            // As soon as you call "ready" function below, the 2 second per turn timer will start.
            game.Ready("MyCSharpBot");

            Log.LogMessage("Successfully created bot! My Player ID is " + game.myId + ". Bot rng seed is " + rngSeed + ".");

            for (; ; )
            {
                game.UpdateFrame();
                Player me = game.me;
                GameMap gameMap = game.gameMap;

                List<Command> commandQueue = new List<Command>();

                foreach (Ship ship in me.ships.Values)
                {
                    if (gameMap.At(ship).halite < Constants.MAX_HALITE / 10 || ship.IsFull())
                    {
                        Direction randomDirection = DirectionExtensions.ALL_CARDINALS[rng.Next(4)];
                        commandQueue.Add(ship.Move(randomDirection));
                    }
                    else
                    {
                        commandQueue.Add(ship.StayStill());
                    }
                }

                if (
                    game.turnNumber <= 200 &&
                    me.halite >= Constants.SHIP_COST &&
                    !gameMap.At(me.shipyard).IsOccupied())
                {
                    commandQueue.Add(me.shipyard.Spawn());
                }

                game.EndTurn(commandQueue);
            }
        }
    }
}
