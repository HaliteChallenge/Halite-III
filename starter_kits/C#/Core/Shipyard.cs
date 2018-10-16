namespace Halite3.Core
{
    public class Shipyard : Entity
    {
        public Shipyard(PlayerId owner, Position position) :
            base(owner, EntityId.NONE, position)
        {
        }

        public Command spawn()
        {
            return Command.spawnShip();
        }
    }
}
