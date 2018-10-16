namespace Halite3.Core
{
    public class Ship : Entity
    {
        public int Halite { get; private set; }

        public Ship(PlayerId owner, EntityId id, Position position, int halite) :
            base(owner, id, position)
        {
            Halite = halite;
        }

        public bool IsFull()
        {
            return Halite >= Constants.MAX_HALITE;
        }

        public Command MakeDropoff()
        {
            return Command.transformShipIntoDropoffSite(Id);
        }

        public Command Move(Direction direction)
        {
            return Command.move(Id, direction);
        }

        public Command StayStill()
        {
            return Command.move(Id, Direction.STILL);
        }

        internal static Ship _generate(PlayerId playerId)
        {
            Input input = Input.readInput();

            EntityId shipId = new EntityId(input.getInt());
            int x = input.getInt();
            int y = input.getInt();
            int halite = input.getInt();

            return new Ship(playerId, shipId, new Position(x, y), halite);
        }

        public override bool Equals(object other)
        {
            if (this == other) return true;
            if (!(other is Ship)) return false;
            if (!base.Equals(other)) return false;

            var ship = other as Ship;

            return Halite == ship.Halite;
        }

        public override int GetHashCode()
        {
            int result = base.GetHashCode();
            result = 31 * result + Halite;
            return result;
        }
    }
}
