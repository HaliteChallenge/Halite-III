using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class Entity
    {
        public  PlayerId owner;
        public  EntityId id;
        public  Position position;

        public Entity( PlayerId owner,  EntityId id,  Position position)
        {
            this.owner = owner;
            this.id = id;
            this.position = position;
        }

        override
        public bool Equals(Object o)
        {
            if (this == o) return true;
            //if (o == null || getClass() != o.getClass()) return false;

            Entity entity = (Entity)o;

            if (!owner.Equals(entity.owner)) return false;
            if (!id.Equals(entity.id)) return false;
            return position.Equals(entity.position);
        }

        override
        public int GetHashCode()
        {
            int result = owner.GetHashCode();
            result = 31 * result + id.GetHashCode();
            result = 31 * result + position.GetHashCode();
            return result;
        }
    }
}
