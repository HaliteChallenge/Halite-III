namespace Halite3.Core
{
    public class MapCell
    {
        public Position Position { get; private set; }
        public int Halite { get; internal set; }
        public Ship Ship { get; internal set; }
        public Entity Structure { get; internal set; }

        public MapCell(Position position, int halite)
        {
            Position = position;
            Halite = halite;
        }

        public bool isEmpty()
        {
            return Ship == null && Structure == null;
        }

        public bool isOccupied()
        {
            return Ship != null;
        }

        public bool hasStructure()
        {
            return Structure != null;
        }

        public void markUnsafe(Ship ship)
        {
            Ship = ship;
        }
    }
}