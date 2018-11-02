using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Halite3.hlt
{
    /// <summary>
    /// The game object holds all metadata pertinent to the game and all its contents
    /// </summary>
    /// <see cref="https://halite.io/learn-programming-challenge/api-docs#game"/>
    public class Game
    {
        public int turnNumber;
        public readonly PlayerId myId;
        public readonly List<Player> players = new List<Player>();
        public readonly Player me;
        public readonly GameMap gameMap;

        /// <summary>
        /// Initiates a game object collecting all start-state instances for the contained items for pre-game.
        /// Also sets up basic logging.
        /// </summary>
        public Game()
        {
            //Grab constants JSON
            Constants.LoadConstants(Input.ReadLine());

            Input inputs = Input.ReadInput();
            int numPlayers = inputs.GetInt();
            myId = new PlayerId(inputs.GetInt());

            Log.Initialize(new StreamWriter(String.Format("bot-{0}.log", myId)));

            for (int i = 0; i < numPlayers; i++)
            {
                players.Add(Player._generate());
            }
            me = players[myId.id];
            gameMap = GameMap._generate();
        }

        /// <summary>
        /// Signals to the Halite engine that you are ready to begin.
        /// </summary>
        /// <param name="name"></param>
        public void Ready(string name)
        {
            Console.WriteLine(name);
        }

        /// <summary>
        /// Reads in the information about the new turn from the Halite engine,
        /// and then updates the GameMap and the Players.
        /// </summary>
        public void UpdateFrame()
        {
            turnNumber = Input.ReadInput().GetInt();
            Log.LogMessage("=============== TURN " + turnNumber + " ================");

            for (int i = 0; i < players.Count; ++i)
            {
                Input input = Input.ReadInput();

                PlayerId currentPlayerId = new PlayerId(input.GetInt());
                int numShips = input.GetInt();
                int numDropoffs = input.GetInt();
                int halite = input.GetInt();

                players[currentPlayerId.id]._update(numShips, numDropoffs, halite);
            }

            gameMap._update();

            foreach (Player player in players)
            {
                foreach (Ship ship in player.ships.Values)
                {
                    gameMap.At(ship).MarkUnsafe(ship);
                }

                gameMap.At(player.shipyard).structure = player.shipyard;

                foreach (Dropoff dropoff in player.dropoffs.Values)
                {
                    gameMap.At(dropoff).structure = dropoff;
                }
            }
        }

        /// <summary>
        /// Converts instances of Command into strings and sends them to the Halite engine.
        /// </summary>
        /// <param name="commands">An IEnumerable such as an array or List of commands.</param>
        public void EndTurn(IEnumerable<Command> commands)
        {
            foreach (Command command in commands)
            {
                Console.Write(command.command);
                Console.Write(' ');
            }
            Console.WriteLine();
        }
    }
}
