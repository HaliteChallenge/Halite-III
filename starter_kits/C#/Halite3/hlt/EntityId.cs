namespace Halite_Project.hlt
{
    public class EntityId
    {
        public static EntityId NONE = new EntityId(-1);

        public int id;

        public EntityId(int id)
        {
            this.id = id;
        }

        public override string ToString()
        {
            return id.ToString();
        }

        public override bool Equals(object o)
        {
            if (this == o) return true;
            if (o == null || !this.GetType().Equals(o.GetType())) return false;

            EntityId entityId = (EntityId)o;

            return id == entityId.id;
        }

        public override int GetHashCode() {
            return id;
        }
    }

}