using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class Game
    {
        public int turnNumber;
        public PlayerId myId;
        public List<Player> players = new List<Player>();
        public Player me;
        public GameMap gameMap;

    public Game()
        {
            Constants.populateConstants(Input.readLine());

             Input input = Input.readInput();
             int numPlayers = input.getInt();
             myId = new PlayerId(input.getInt());

            Constants.setID(myId.id);

            for (int i = 0; i < numPlayers; ++i)
            {
                players.Add(Player._generate());
            }
            me = players[myId.id];
            gameMap = GameMap._generate();
        }

        public void ready( string name)
        {
            Console.WriteLine(name);
        }

        public void updateFrame()
        {
            turnNumber = Input.readInput().getInt();
            Log.log("=============== TURN " + turnNumber + " ================");

            for (int i = 0; i < players.Count; ++i)
            {
                 Input input = Input.readInput();

                 PlayerId currentPlayerId = new PlayerId(input.getInt());
                 int numShips = input.getInt();
                 int numDropoffs = input.getInt();
                 int halite = input.getInt();

                players[currentPlayerId.id]._update(numShips, numDropoffs, halite);
            }

            gameMap._update();

            foreach ( Player player in players)
            {
                foreach ( Ship ship in player.ships.Values)
                {
                    gameMap.at(ship).markUnsafe(ship);
                }

                gameMap.at(player.shipyard).structure = player.shipyard;

                foreach ( Dropoff dropoff in player.dropoffs.Values)
                {
                    gameMap.at(dropoff).structure = dropoff;
                }
            }
        }

        public void endTurn( List<Command> commands)
        {
            foreach ( Command command in commands)
            {
                Console.WriteLine(command.command);
                Console.WriteLine(' ');
            }
            Console.WriteLine();
        }
    }
}
