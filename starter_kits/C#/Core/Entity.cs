namespace Halite3.Core
{
    public class Entity
    {
        public PlayerId Owner { get; private set; }
        public EntityId Id { get; private set; }
        public Position Position { get; private set; }

        public Entity(PlayerId owner, EntityId id, Position position)
        {
            Owner = owner;
            Id = id;
            Position = position;
        }

        public override bool Equals(object other)
        {
            if (this == other) return true;
            if (!(other is Entity)) return false;

            var entity = other as Entity;

            if (!Owner.Equals(entity.Owner)) return false;
            if (!Id.Equals(entity.Id)) return false;
            return Position.Equals(entity.Position);
        }

        public override int GetHashCode()
        {
            int result = Owner.GetHashCode();
            result = 31 * result + Id.GetHashCode();
            result = 31 * result + Position.GetHashCode();
            return result;
        }
    }
}