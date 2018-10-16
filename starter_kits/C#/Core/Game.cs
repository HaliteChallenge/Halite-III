using System;
using System.Collections.Generic;

namespace Halite3.Core
{
    public class Game
    {
        public int TurnNumber { get; private set; }
        public PlayerId MyId { get; private set; }
        public Dictionary<int, Player> Players { get; private set; }
        public Player Me { get; private set; }
        public GameMap GameMap { get; private set; }

        public Game()
        {
            Players = new Dictionary<int, Player>();

            Constants.populateConstants(Input.ReadLine());

            Input input = Input.readInput();
            int numPlayers = input.getInt();
            MyId = new PlayerId(input.getInt());

            Log.logger().Info("My Id: " + MyId.Id);

            for (var i = 0; i < numPlayers; ++i)
            {
                var player = Player._generate();
                Players.Add(player.Id.Id, player);
            }

            Me = Players[MyId.Id];
            GameMap = GameMap._generate();
        }

        public void Ready(string name)
        {
            Console.WriteLine(name);
        }

        public void UpdateFrame()
        {
            TurnNumber = Input.readInput().getInt();
            Log.logger().Info("=============== TURN " + TurnNumber + " ================");

            for (int i = 0; i < Players.Count; ++i)
            {
                Input input = Input.readInput();

                PlayerId currentPlayerId = new PlayerId(input.getInt());
                int numShips = input.getInt();
                int numDropoffs = input.getInt();
                int halite = input.getInt();

                Players[currentPlayerId.Id]._update(numShips, numDropoffs, halite);
            }

            GameMap._update();

            foreach (var player in Players.Values)
            {
                foreach (var ship in player.Ships.Values)
                {
                    GameMap.At(ship).markUnsafe(ship);
                }

                GameMap.At(player.Shipyard).Structure = player.Shipyard;

                foreach (var dropoff in player.Dropoffs.Values)
                {
                    GameMap.At(dropoff).Structure = dropoff;
                }
            }
        }

        public void EndTurn(List<Command> commands)
        {
            foreach (var command in commands)
            {
                Console.WriteLine(command.CommandValue);
                Console.WriteLine(' ');
            }
            Console.WriteLine();
        }
    }
}