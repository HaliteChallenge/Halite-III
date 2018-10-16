using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class Ship : Entity
    {
        public int halite;

        public Ship( PlayerId owner,  EntityId id,  Position position,  int halite)
			: base(owner, id, position)
        {
            this.halite = halite;
        }

        public bool isFull()
        {
            return halite >= Constants.MAX_HALITE;
        }

        public Command makeDropoff()
        {
            return Command.transformShipIntoDropoffSite(id);
        }

        public Command move( Direction direction)
        {
            return Command.move(id, direction);
        }

        public Command stayStill()
        {
            return Command.move(id, new STILL());
        }

        public static Ship _generate( PlayerId playerId)
        {
             Input input = Input.readInput();

             EntityId shipId = new EntityId(input.getInt());
             int x = input.getInt();
             int y = input.getInt();
             int halite = input.getInt();

            return new Ship(playerId, shipId, new Position(x, y), halite);
        }


        public override bool Equals(Object o)
        {
            if (this == o) return true;
            //if (o == null || getClass() != o.getClass()) return false;
            if (!Equals(o)) return false;

            Ship ship = (Ship)o;

            return halite == ship.halite;
        }

        public override int GetHashCode()
        {
            int result = GetHashCode();
            result = 31 * result + halite;
            return result;
        }
    }
}
