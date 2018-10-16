namespace Halite3.Core
{
    public class EntityId
    {
        public static EntityId NONE = new EntityId(-1);

        public int Id { get; private set; }

        public EntityId(int id)
        {
            Id = id;
        }

        public override bool Equals(object other)
        {
            if (this == other) return true;
            if (!(other is EntityId)) return false;

            var entityId = other as EntityId;

            return Id == entityId.Id;
        }

        public override int GetHashCode()
        {
            return Id;
        }
    }
}