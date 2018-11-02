namespace Halite3.hlt
{
    /// <summary>
    /// A ship is a type of Entity and is used to collect and transport halite.
    /// <para>
    /// Has a max halite capacity of 1000. Can move once per turn.
    /// </para>
    /// </summary>
    /// <see cref="https://halite.io/learn-programming-challenge/api-docs#ship"></see>
    public class Ship : Entity
    {
        public readonly int halite;

        public Ship(PlayerId owner, EntityId id, Position position, int halite) : base(owner, id, position)
        {
            this.halite = halite;
        }

        /// <summary>
        /// Returns true if this ship is carrying the max amount of halite possible.
        /// </summary>
        public bool IsFull()
        {
            return halite >= Constants.MAX_HALITE;
        }

        /// <summary>
        /// Returns the command to turn this ship into a dropoff.
        /// </summary>
        public Command MakeDropoff()
        {
            return Command.TransformShipIntoDropoffSite(id);
        }

        /// <summary>
        /// Returns the command to move this ship in a direction.
        /// </summary>
        public Command Move(Direction direction)
        {
            return Command.Move(id, direction);
        }

        /// <summary>
        /// Returns the command to keep this ship still.
        /// </summary>
        public Command StayStill()
        {
            return Command.Move(id, Direction.STILL);
        }

        /// <summary>
        /// Reads in the details of a new ship from the Halite engine.
        /// </summary>
        public static Ship _generate(PlayerId playerId)
        {
            Input input = Input.ReadInput();

            EntityId shipId = new EntityId(input.GetInt());
            int x = input.GetInt();
            int y = input.GetInt();
            int halite = input.GetInt();

            return new Ship(playerId, shipId, new Position(x, y), halite);
        }

        public override bool Equals(object obj)
        {
            if (this == obj)
                return true;
            if (obj == null || this.GetType() != obj.GetType())
                return false;
            if (!base.Equals(obj)) return false;

            Ship ship = (Ship)obj;

            return halite == ship.halite;
        }

        public override int GetHashCode()
        {
            int result = base.GetHashCode();
            result = 31 * result + halite;
            return result;
        }
    }
}
