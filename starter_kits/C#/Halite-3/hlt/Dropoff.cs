using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class Dropoff : Entity
    {
        public Dropoff( PlayerId owner,  EntityId id,  Position position)
			: base(owner, id, position)
        {
            this.owner = owner;
            this.id = id;
            this.position = position;
        }

        public static Dropoff _generate( PlayerId playerId)
        {
             Input input = Input.readInput();

             EntityId dropoffId = new EntityId(input.getInt());
             int x = input.getInt();
             int y = input.getInt();

            return new Dropoff(playerId, dropoffId, new Position(x, y));
        }
    }
}
