namespace Halite_Project.hlt
{
    public class Dropoff : Entity
    {
        public Dropoff(PlayerId owner, EntityId id, Position position) : base(owner, id, position) { }

        public static Dropoff _generate(PlayerId playerId)
        {
            Input input = Input.ReadInput();

            EntityId dropoffId = new EntityId(input.GetInt());
            int x = input.GetInt();
            int y = input.GetInt();

            return new Dropoff(playerId, dropoffId, new Position(x, y));
        }
    }
}