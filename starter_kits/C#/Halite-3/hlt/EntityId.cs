using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class EntityId
    {
        public static  EntityId NONE = new EntityId(-1);

        public  int id;

        public EntityId(int id)
        {
            this.id = id;
        }

        override
        public string ToString()
        {
            return id.ToString();
        }

        override
        public bool Equals(Object o)
        {
            if (this == o) return true;
            //if (o == null || getClass() != o.getClass()) return false;

            EntityId entityId = (EntityId)o;

            return id == entityId.id;
        }

        override
        public int GetHashCode()
        {
            return id;
        }
    }
}
