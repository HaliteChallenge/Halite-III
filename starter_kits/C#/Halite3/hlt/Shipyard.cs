namespace Halite_Project.hlt {
    public class Shipyard : Entity {
        public Shipyard(PlayerId owner, Position position) : base(owner, EntityId.NONE, position) {
            
        }

        public Command Spawn() {
            return Command.SpawnShip();
        }
    }
}