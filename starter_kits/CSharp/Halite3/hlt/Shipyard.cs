namespace Halite3.hlt
{
    /// <summary>
    /// A type of Entity that can be used to spawn Ships.
    /// </summary>
    /// <see cref="https://halite.io/learn-programming-challenge/api-docs#shipyard"/>
    public class Shipyard : Entity
    {
        public Shipyard(PlayerId owner, Position position) : base(owner, new EntityId(EntityId.NONE), position)
        {
        }

        /// <summary>
        /// Returns the command to spawn a new ship
        /// </summary>
        public Command Spawn()
        {
            return Command.SpawnShip();
        }
    }
}
