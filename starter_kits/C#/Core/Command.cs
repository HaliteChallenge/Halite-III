namespace Halite3.Core
{
    public class Command
    {
        public string CommandValue { get; private set; }

        public static Command spawnShip()
        {
            return new Command("g");
        }

        public static Command transformShipIntoDropoffSite(EntityId id)
        {
            return new Command("c " + id.Id);
        }

        public static Command move(EntityId id, Direction direction)
        {
            return new Command("m " + id.Id + ' ' + (char)direction);
        }

        private Command(string command)
        {
            this.CommandValue = command;
        }

        public override bool Equals(object other)
        {
            if (this == other) return true;
            if (!(other is Command)) return false;

            var otherCommand = other as Command;

            return CommandValue.Equals(otherCommand.CommandValue);
        }

        public override int GetHashCode()
        {
            return CommandValue.GetHashCode();
        }
    }
}