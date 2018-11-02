using System;
using System.Collections.Generic;
using System.Text;

namespace Halite3.hlt
{
    /// <summary>
    /// A base class extended by Ship, Dropoff, and Shipyard.
    /// </summary>
    public class Entity
    {
        public readonly PlayerId owner;
        public readonly EntityId id;
        public Position position;

        public Entity(PlayerId owner, EntityId id, Position position)
        {
            this.owner = owner;
            this.id = id;
            this.position = position;
        }

        public override string ToString()
        {
            return String.Format("{0}(id={1}, {2}", this.GetType(), this.id, this.position);
        }

        public override bool Equals(object obj)
        {
            if (this == obj)
                return true;
            if (obj == null || this.GetType() != obj.GetType())
                return false;

            Entity entity = (Entity)obj;

            if (!owner.Equals(entity.owner))
                return false;
            if (!id.Equals(entity.id))
                return false;
            return position.Equals(entity.position);
        }

        public override int GetHashCode()
        {
            int result = owner.GetHashCode();
            result = 31 * result + id.GetHashCode();
            result = 31 * result + position.GetHashCode();
            return result;
        }
    }
}
