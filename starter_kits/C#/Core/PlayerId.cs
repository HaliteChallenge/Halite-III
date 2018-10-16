namespace Halite3.Core
{
    public class PlayerId
    {
        public int Id { get; private set; }

        public PlayerId(int id)
        {
            Id = id;
        }

        public override bool Equals(object other)
        {
            if (this == other) return true;
            if (!(other is PlayerId)) return false;

            var playerId = other as PlayerId;

            return Id == playerId.Id;
        }

        public override int GetHashCode()
        {
            return Id;
        }
    }
}
