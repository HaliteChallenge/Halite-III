namespace Halite_Project.hlt
{
    public class Entity
    {
        public PlayerId owner;
        public EntityId id;
        public Position position;

        public Entity(PlayerId owner, EntityId id, Position position)
        {
            this.owner = owner;
            this.id = id;
            this.position = position;
        }

        public virtual bool equals(object o)
        {
            if (this == o) return true;
            if (o == null || !this.GetType().Equals(o.GetType())) return false;

            Entity entity = (Entity)o;

            if (!owner.Equals(entity.owner)) return false;
            if (!id.Equals(entity.id)) return false;
            return position.Equals(entity.position);
        }

        public virtual int hashCode()
        {
            int result = owner.GetHashCode();
            result = 31 * result + id.GetHashCode();
            result = 31 * result + position.GetHashCode();
            return result;
        }
    }
}