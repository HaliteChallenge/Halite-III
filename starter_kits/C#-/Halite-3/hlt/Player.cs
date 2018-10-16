using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class Player
    {
        public  PlayerId id;
        public  Shipyard shipyard;
        public int halite;
        public Dictionary<EntityId, Ship> ships = new Dictionary<EntityId, Ship>();
        public Dictionary<EntityId, Dropoff> dropoffs = new Dictionary<EntityId, Dropoff>();

        private Player( PlayerId id,  Shipyard shipyard)
        {
            this.id = id;
            this.shipyard = shipyard;
        }

        public void _update( int numShips,  int numDropoffs,  int halite)
        {
            this.halite = halite;

            ships.Clear();
            for (int i = 0; i < numShips; ++i)
            {
                 Ship ship = Ship._generate(id);
                ships.Add(ship.id, ship);
            }

            dropoffs.Clear();
            for (int i = 0; i < numDropoffs; ++i)
            {
                 Dropoff dropoff = Dropoff._generate(id);
                dropoffs.Add(dropoff.id, dropoff);
            }
        }

        public static Player _generate()
        {
             Input input = Input.readInput();

             PlayerId playerId = new PlayerId(input.getInt());
             int shipyard_x = input.getInt();
             int shipyard_y = input.getInt();

            return new Player(playerId, new Shipyard(playerId, new Position(shipyard_x, shipyard_y)));
        }
    }
}
