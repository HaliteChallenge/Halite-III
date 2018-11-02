using System;
using System.Collections.Generic;
using System.Text;

namespace Halite3.hlt
{
    /// <summary>
    /// An EntityId is the identifier that corresponds to a ship, shipyard, or dropoff.
    /// </summary>
    public class EntityId
    {
        public const int NONE = -1;
        public readonly int id;

        public EntityId(int id)
        {
            this.id = id;
        }

        public override string ToString()
        {
            return id.ToString();
        }

        public override bool Equals(object obj)
        {
            if (this == obj)
                return true;
            if (obj == null || this.GetType() != obj.GetType())
                return false;

            EntityId entityId = (EntityId)obj;

            return id == entityId.id;
        }

        public override int GetHashCode()
        {
            return id;
        }
    }
}
