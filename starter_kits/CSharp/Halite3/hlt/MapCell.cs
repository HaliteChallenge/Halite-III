namespace Halite3.hlt
{
    /// <summary>
    /// A map cell is an object representation of a cell on the game map.
    /// Map cell has position, halite, ship, and structure as member variables.
    /// </summary>
    /// <see cref="https://halite.io/learn-programming-challenge/api-docs#map-cell"/>
    public class MapCell
    {
        public readonly Position position;
        public int halite;
        public Ship ship;
        public Entity structure;

        public MapCell(Position position, int halite)
        {
            this.position = position;
            this.halite = halite;
        }

        /// <summary>
        /// Returns true if there is neither a ship nor a structure on this MapCell.
        /// </summary>
        public bool IsEmpty()
        {
            return ship == null && structure == null;
        }

        /// <summary>
        /// Returns true if there is a ship on this MapCell.
        /// </summary>
        public bool IsOccupied()
        {
            return ship != null;
        }

        /// <summary>
        /// Returns true if there is a structure on this MapCell.
        /// </summary>
        public bool HasStructure()
        {
            return structure != null;
        }

        /// <summary>
        /// Is used to mark the cell under this ship as unsafe (occupied) for collision avoidance.
        /// <para>
        /// This marking resets every turn and is used by NaiveNavigate to avoid collisions.
        /// </para>
        /// </summary>
        /// <seealso cref="GameMap.NaiveNavigate(Ship, Position)"/>
        public void MarkUnsafe(Ship ship)
        {
            this.ship = ship;
        }
    }
}
