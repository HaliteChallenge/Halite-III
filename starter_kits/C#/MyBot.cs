using System;
using System.Collections.Generic;
using System.Diagnostics;
using Halite3.Core;
using NLog;

namespace Halite3
{
    class MyBot
    {
        static void Main(string[] args)
        {
            Random rng = new Random(Guid.NewGuid().GetHashCode());

            // Uncomment below to attach debugger
            // while(!Debugger.IsAttached);

            Game game = new Game();
            // At this point "game" variable is populated with initial map data.
            // This is a good place to do computationally expensive start-up pre-processing.
            // As soon as you call "ready" function below, the 2 second per turn timer will start.
            game.Ready("PopAye");

            Log.logger().Info("Successfully created bot! My Player ID is " + game.MyId + ".");

            var allDirections = new List<Direction> { Direction.NORTH, Direction.EAST, Direction.SOUTH, Direction.WEST };

            while (true)
            {
                game.UpdateFrame();
                var me = game.Me;
                GameMap gameMap = game.GameMap;

                var commandQueue = new List<Command>();

                foreach (var ship in me.Ships.Values)
                {
                    if (gameMap.At(ship).Halite < Constants.MAX_HALITE / 10 || ship.IsFull())
                    {
                        var randomDirection = allDirections[rng.Next(4)];
                        commandQueue.Add(ship.Move(randomDirection));
                    }
                    else
                    {
                        commandQueue.Add(ship.StayStill());
                    }
                }

                if (
                    game.TurnNumber <= 200 &&
                    me.Halite >= Constants.SHIP_COST &&
                    !gameMap.At(me.Shipyard).isOccupied())
                {
                    commandQueue.Add(me.Shipyard.spawn());
                }

                game.EndTurn(commandQueue);
            }
        }
    }
}
