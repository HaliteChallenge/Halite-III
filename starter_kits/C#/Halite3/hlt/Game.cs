using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Halite_Project.hlt {
    class Game {
        public int turnNumber;
        public PlayerId myId;
        public List<Player> players = new List<Player>();
        public Player me;
        public GameMap gameMap;

        public Game() {
            Constants.PopulateConstants(Input.ReadLine());

            Input input = Input.ReadInput();
            int numPlayers = input.GetInt();
            myId = new PlayerId(input.GetInt());

            Log.Open(myId.id);

            for(int i = 0; i < numPlayers; i++) {
                players.Add(Player._generate());
            }

            me = players[myId.id];
            gameMap = GameMap._generate();
        }

        public void Ready(string name) {
            Console.WriteLine(name);
        }

        public void UpdateFrame() {
            turnNumber = Input.ReadInput().GetInt();
            Log.CreateLog("=============== TURN " + turnNumber + " ================");

            for(int i = 0; i < players.Count; i++) {
                Input input = Input.ReadInput();

                PlayerId currentPlayerId = new PlayerId(input.GetInt());
                int numShips = input.GetInt();
                int numDropoffs = input.GetInt();
                int halite = input.GetInt();

                players[currentPlayerId.id]._update(numShips, numDropoffs, halite);
            }

            gameMap._update();

            foreach(Player player in players) {
                foreach(Ship ship in player.ships.Values) {
                    gameMap.At(ship).MarkUnsafe(ship);
                }

                gameMap.At(player.shipyard).structure = player.shipyard;

                foreach(Dropoff dropoff in player.dropoffs.Values){
                    gameMap.At(dropoff).structure = dropoff;
                }
            }
        }

        public void EndTurn(List<Command> commands) {
            foreach(Command command in commands) {
                Console.Write(command.command);
                Console.Write(" ");
            }

            Console.WriteLine("");
        }
    }
}
