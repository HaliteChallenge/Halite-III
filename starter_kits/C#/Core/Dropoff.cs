namespace Halite3.Core
{
    public class Dropoff : Entity
    {
        public Dropoff(PlayerId owner, EntityId id, Position position) :
            base(owner, id, position)
        {
        }

        internal static Dropoff _generate(PlayerId playerId)
        {
            Input input = Input.readInput();

            EntityId dropoffId = new EntityId(input.getInt());
            int x = input.getInt();
            int y = input.getInt();

            return new Dropoff(playerId, dropoffId, new Position(x, y));
        }
    }
}
