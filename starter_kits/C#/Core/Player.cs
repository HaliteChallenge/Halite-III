using System.Collections.Generic;

namespace Halite3.Core
{
    public class Player
    {
        public PlayerId Id { get; private set; }
        public Shipyard Shipyard { get; private set; }
        public int Halite { get; private set; }
        public Dictionary<EntityId, Ship> Ships { get; private set; }
        public Dictionary<EntityId, Dropoff> Dropoffs { get; private set; }

        private Player(PlayerId id, Shipyard shipyard)
        {
            Id = id;
            Shipyard = shipyard;
            Ships = new Dictionary<EntityId, Ship>();
            Dropoffs = new Dictionary<EntityId, Dropoff>();
        }

        internal void _update(int numShips, int numDropoffs, int halite)
        {
            Halite = halite;

            Ships.Clear();
            for (var i = 0; i < numShips; ++i)
            {
                Ship ship = Ship._generate(Id);
                Ships.Add(ship.Id, ship);
            }

            Dropoffs.Clear();
            for (var i = 0; i < numDropoffs; ++i)
            {
                Dropoff dropoff = Dropoff._generate(Id);
                Dropoffs.Add(dropoff.Id, dropoff);
            }
        }

        internal static Player _generate()
        {
            var input = Input.readInput();

            var playerId = new PlayerId(input.getInt());
            int shipyard_x = input.getInt();
            int shipyard_y = input.getInt();

            return new Player(playerId, new Shipyard(playerId, new Position(shipyard_x, shipyard_y)));
        }
    }
}