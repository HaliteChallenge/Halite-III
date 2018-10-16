using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class Shipyard : Entity
    {
        public Shipyard(PlayerId owner, Position position)
			: base(owner, EntityId.NONE, position)
        {
            
        }

        public Command spawn()
        {
            return Command.spawnShip();
        }
    }
}
