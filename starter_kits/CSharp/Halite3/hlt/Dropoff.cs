namespace Halite3.hlt
{
    /// <summary>
    /// A Dropoff is a type of Entity where allied ships can drop off halite.
    /// </summary>
    /// <see cref="https://halite.io/learn-programming-challenge/api-docs#dropoff"/>
    public class Dropoff : Entity
    {
        public Dropoff(PlayerId owner, EntityId id, Position position) : base(owner, id, position)
        {
        }

        /// <summary>
        /// Creates a Dropoff instance based on engine output.
        /// </summary>
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
