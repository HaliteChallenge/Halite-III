namespace Halite_Project.hlt
{
    public class MapCell
    {
        public Position position;
        public int halite;
        public Ship ship;
        public Entity structure;

        public MapCell(Position position, int halite)
        {
            this.position = position;
            this.halite = halite;
        }

        public bool isEmpty()
        {
            return ship == null && structure == null;
        }

        public bool IsOccupied()
        {
            return ship != null;
        }

        public bool hasStructure()
        {
            return structure != null;
        }

        public void MarkUnsafe(Ship ship)
        {
            this.ship = ship;
        }
    }

}