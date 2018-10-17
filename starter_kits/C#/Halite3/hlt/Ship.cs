namespace Halite_Project.hlt
{
    public class Ship : Entity
    {
        public int halite;

        public Ship(PlayerId owner, EntityId id, Position position, int halite) : base(owner, id, position) {
            this.halite = halite;
        }

        public bool IsFull()
        {
            return halite >= Constants.MAX_HALITE;
        }

        public Command MakeDropoff()
        {
            return Command.TransformShipIntoDropoffSite(id);
        }

        public Command Move(EDirection direction)
        {
            return Command.Move(id, direction);
        }

        public Command StayStill()
        {
            return Command.Move(id, EDirection.STILL);
        }

        public static Ship _generate(PlayerId playerId)
        {
            Input input = Input.ReadInput();

            EntityId shipId = new EntityId(input.GetInt());
            int x = input.GetInt();
            int y = input.GetInt();
            int halite = input.GetInt();

            return new Ship(playerId, shipId, new Position(x, y), halite);
        }

        public override bool Equals(object o) {
            if (this == o) return true;
            if (o == null || !this.GetType().Equals(o.GetType())) return false;
            if (!base.equals(o)) return false;

            Ship ship = (Ship)o;

            return halite == ship.halite;
        }
        
        public override int GetHashCode()
        {
            int result = base.hashCode();
            result = 31 * result + halite;
            return result;
        }
    }

}
