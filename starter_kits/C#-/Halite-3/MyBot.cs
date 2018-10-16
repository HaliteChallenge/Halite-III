using System;
using MyBot.hlt;
using System.Security.Cryptography;
using System.Collections;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.IO;

namespace MyBot
{
    class Program
    {
        static int getRandom(int max)
        {
            RNGCryptoServiceProvider provider = new RNGCryptoServiceProvider();
            var byteArray = new byte[4];
            provider.GetBytes(byteArray);

            //convert 4 bytes to an integer
            var randomInteger = BitConverter.ToInt32(byteArray, 0);

            randomInteger = (int)((float)randomInteger / (float)int.MaxValue) * max;

            return randomInteger;
        }

        static void Main(string[] args)
        {
            Game game = new Game();
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
                        Direction randomDirection = Direction.ALL_CARDINALS[getRandom(4)];
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
