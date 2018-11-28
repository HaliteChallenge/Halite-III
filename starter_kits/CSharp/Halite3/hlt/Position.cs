using System;
using System.Collections.Generic;
using static Halite3.hlt.Direction;

namespace Halite3.hlt
{
    /// <summary>
    /// A position is an object with x and y values indicating the absolute position on the game map.
    /// </summary>
    /// <see cref="https://halite.io/learn-programming-challenge/api-docs#position"/>
    public class Position
    {
        public readonly int x;
        public readonly int y;

        public Position(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        /// <summary>
        /// Returns a new position based on moving one unit in the given direction from the given position.
        /// Does not account for toroidal wraparound, that's done in GameMap.
        /// </summary>
        /// <seealso cref="GameMap.Normalize(Position)"/>
        public Position DirectionalOffset(Direction d)
        {
            int dx;
            int dy;

            switch (d)
            {
                case NORTH:
                    dx = 0;
                    dy = -1;
                    break;

                case SOUTH:
                    dx = 0;
                    dy = 1;
                    break;

                case EAST:
                    dx = 1;
                    dy = 0;
                    break;

                case WEST:
                    dx = -1;
                    dy = 0;
                    break;

                case STILL:
                    dx = 0;
                    dy = 0;
                    break;

                default:
                    throw new InvalidOperationException("Unknown direction " + d);
            }

            return new Position(x + dx, y + dy);
        }

        /// <summary>
        /// Returns a list of all positions around the given position in each cardinal direction
        /// </summary>
        public List<Position> GetSurroundingCardinals()
        {
            List<Position> positions = new List<Position>();
            positions.Add(DirectionalOffset(Direction.NORTH));
            positions.Add(DirectionalOffset(Direction.EAST));
            positions.Add(DirectionalOffset(Direction.SOUTH));
            positions.Add(DirectionalOffset(Direction.WEST));
            return positions;
        }
    }
}
