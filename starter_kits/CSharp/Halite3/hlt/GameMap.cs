using System;
using System.Collections.Generic;

namespace Halite3.hlt
{
    /// <summary>
    /// Gameplay takes place on a wrapping rectangular grid 32x32, 40x40, 48x48, 56x56, or 64x64 in dimension.
    /// <para>
    /// This class is responsible for two key functions:
    /// keeping track of what's on the map at any given cell, and helping ships navigate.
    /// </para>
    /// </summary>
    /// <para><see cref="https://halite.io/learn-programming-challenge/api-docs#map"/></para>
    public class GameMap
    {
        public readonly int width;
        public readonly int height;
        public readonly MapCell[][] cells;

        /// <summary>
        /// Creates a new instance of a GameMap
        /// </summary>
        /// <para><seealso cref="_generate"/></para>
        /// <param name="width">The width, as an int, of the map</param>
        /// <param name="height">The height, as an int, of the map</param>
        public GameMap(int width, int height)
        {
            this.width = width;
            this.height = height;

            cells = new MapCell[height][];
            for (int y = 0; y < height; ++y)
            {
                cells[y] = new MapCell[width];
            }
        }

        /// <summary>
        /// Normalizes the given Position and then returns the corresponding MapCell.
        /// </summary>
        public MapCell At(Position position)
        {
            Position normalized = Normalize(position);
            return cells[normalized.y][normalized.x];
        }

        /// <summary>
        /// Normalizes the position of an Entity and returns the corresponding MapCell.
        /// </summary>
        public MapCell At(Entity entity)
        {
            return At(entity.position);
        }

        /// <summary>
        /// A method that computes the Manhattan distance between two locations, and accounts for the toroidal wraparound.
        /// </summary>
        public int CalculateDistance(Position source, Position target)
        {
            Position normalizedSource = Normalize(source);
            Position normalizedTarget = Normalize(target);

            int dx = Math.Abs(normalizedSource.x - normalizedTarget.x);
            int dy = Math.Abs(normalizedSource.y - normalizedTarget.y);

            int toroidal_dx = Math.Min(dx, width - dx);
            int toroidal_dy = Math.Min(dy, height - dy);

            return toroidal_dx + toroidal_dy;
        }

        /// <summary>
        /// A method that normalizes a position within the bounds of the toroidal map.
        /// </summary>
        /// <remarks>
        /// Useful for handling the wraparound modulus arithmetic on x and y.
        /// For example, if a ship at (x = 31, y = 4) moves to the east on a 32x32 map,
        /// the normalized position would be (x = 0, y = 4), rather than the off-the-map position of (x = 32, y = 4).
        /// </remarks>
        public Position Normalize(Position position)
        {
            int x = ((position.x % width) + width) % width;
            int y = ((position.y % height) + height) % height;
            return new Position(x, y);
        }

        /// <summary>
        /// A method that returns a list of direction(s) to move closer to a target disregarding collision possibilities.
        /// Returns an empty list if the source and destination are the same.
        /// </summary>
        public List<Direction> GetUnsafeMoves(Position source, Position destination)
        {
            List<Direction> possibleMoves = new List<Direction>();

            Position normalizedSource = Normalize(source);
            Position normalizedDestination = Normalize(destination);

            int dx = Math.Abs(normalizedSource.x - normalizedDestination.x);
            int dy = Math.Abs(normalizedSource.y - normalizedDestination.y);
            int wrapped_dx = width - dx;
            int wrapped_dy = height - dy;

            if (normalizedSource.x < normalizedDestination.x)
            {
                possibleMoves.Add(dx > wrapped_dx ? Direction.WEST : Direction.EAST);
            }
            else if (normalizedSource.x > normalizedDestination.x)
            {
                possibleMoves.Add(dx < wrapped_dx ? Direction.WEST : Direction.EAST);
            }

            if (normalizedSource.y < normalizedDestination.y)
            {
                possibleMoves.Add(dy > wrapped_dy ? Direction.NORTH : Direction.SOUTH);
            }
            else if (normalizedSource.y > normalizedDestination.y)
            {
                possibleMoves.Add(dy < wrapped_dy ? Direction.NORTH : Direction.SOUTH);
            }

            return possibleMoves;
        }

        /// <summary>
        /// A method that returns a direction to move closer to a target without colliding with other entities.
        /// Returns a direction of “still” if no such move exists.
        /// </summary>
        public Direction NaiveNavigate(Ship ship, Position destination)
        {
            // getUnsafeMoves normalizes for us
            foreach (Direction direction in GetUnsafeMoves(ship.position, destination))
            {
                Position targetPos = ship.position.DirectionalOffset(direction);
                if (!At(targetPos).IsOccupied())
                {
                    At(targetPos).MarkUnsafe(ship);
                    return direction;
                }
            }

            return Direction.STILL;
        }

        /// <summary>
        /// Clears all the ships in preparation for player._update() and updates the halite on each cell.
        /// </summary>
        public void _update()
        {
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    cells[y][x].ship = null;
                }
            }

            int updateCount = Input.ReadInput().GetInt();

            for (int i = 0; i < updateCount; ++i)
            {
                Input input = Input.ReadInput();
                int x = input.GetInt();
                int y = input.GetInt();

                cells[y][x].halite = input.GetInt();
            }
        }

        /// <summary>
        /// Reads the starting map for the game from the Halite engine.
        /// </summary>
        /// <returns></returns>
        public static GameMap _generate()
        {
            Input mapInput = Input.ReadInput();
            int width = mapInput.GetInt();
            int height = mapInput.GetInt();

            GameMap map = new GameMap(width, height);

            for (int y = 0; y < height; ++y)
            {
                Input rowInput = Input.ReadInput();

                for (int x = 0; x < width; ++x)
                {
                    int halite = rowInput.GetInt();
                    map.cells[y][x] = new MapCell(new Position(x, y), halite);
                }
            }

            return map;
        }
    }
}
