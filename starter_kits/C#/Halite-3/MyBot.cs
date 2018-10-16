using System;
using MyBot.hlt;
using System.Collections.Generic;

namespace MyBot
{
    class Program
    {
        static int rngSeed;

        static void Main(string[] args)
        {
            Game game = new Game();
            if (args.Length > 1)
            {
                rngSeed = int.Parse(args[1]);
            }
            else
            {
                rngSeed = DateTime.Now.Millisecond;
            }
            Random rng = new Random(rngSeed);
            // At this point "game" variable is populated with initial map data.
            // This is a good place to do computationally expensive start-up pre-processing.
            // As soon as you call "ready" function below, the 2 second per turn timer will start.
            game.ready("MyC#Bot");

            Log.log("Successfully created bot! My Player ID is " + game.myId + ". Bot rng seed is NA.");

			while (true)
            {
                game.updateFrame();
                Player me = game.me;
                GameMap gameMap = game.gameMap;
           
                List<Command> commandQueue = new List<Command>();

                foreach ( Ship ship in me.ships.Values)
                {
                    if (gameMap.at(ship).halite < Constants.MAX_HALITE / 10 || ship.isFull())
                    {
                        Direction randomDirection = Direction.ALL_CARDINALS[rng(4)];
                        commandQueue.Add(ship.move(randomDirection));
                    }
                    else
                    {
                        commandQueue.Add(ship.stayStill());
                    }
                }

                if (
                    game.turnNumber <= 200 &&
                    me.halite >= Constants.SHIP_COST &&
                    !gameMap.at(me.shipyard).isOccupied())
                {
                    commandQueue.Add(me.shipyard.spawn());
                }

                game.endTurn(commandQueue);
            }
        }
    }
}
