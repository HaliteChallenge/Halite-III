using System;
using System.Collections.Generic;
using Halite3.Core.Utils;

namespace Halite3.Core
{
    public class GameMap
    {
        public int Width { get; private set; }
        public int Height { get; private set; }
        public MapCell[,] Cells { get; private set; }

        public GameMap(int width, int height)
        {
            Width = width;
            Height = height;

            Cells = new MapCell[width, height];
        }

        public MapCell this[int x, int y] => Cells[x.Mod(Width), y.Mod(Height)];

        public MapCell At(Position position)
        {
            Position normalized = normalize(position);
            return Cells[normalized.X, normalized.Y];
        }

        public MapCell At(Entity entity)
        {
            return At(entity.Position);
        }

        public int CalculateDistance(Position source, Position target)
        {
            Position normalizedSource = normalize(source);
            Position normalizedTarget = normalize(target);

            int dx = Math.Abs(normalizedSource.X - normalizedTarget.X);
            int dy = Math.Abs(normalizedSource.Y - normalizedTarget.Y);

            int toroidal_dx = Math.Min(dx, Width - dx);
            int toroidal_dy = Math.Min(dy, Height - dy);

            return toroidal_dx + toroidal_dy;
        }

        public Position normalize(Position position)
        {
            int x = ((position.X % Width) + Width) % Width;
            int y = ((position.Y % Height) + Height) % Height;
            return new Position(x, y);
        }

        public List<Direction> getUnsafeMoves(Position source, Position destination)
        {
            var possibleMoves = new List<Direction>();

            Position normalizedSource = normalize(source);
            Position normalizedDestination = normalize(destination);

            int dx = Math.Abs(normalizedSource.X - normalizedDestination.X);
            int dy = Math.Abs(normalizedSource.Y - normalizedDestination.Y);
            int wrapped_dx = Width - dx;
            int wrapped_dy = Height - dy;

            if (normalizedSource.X < normalizedDestination.X)
            {
                possibleMoves.Add(dx > wrapped_dx ? Direction.WEST : Direction.EAST);
            }
            else if (normalizedSource.X > normalizedDestination.X)
            {
                possibleMoves.Add(dx < wrapped_dx ? Direction.WEST : Direction.EAST);
            }

            if (normalizedSource.Y < normalizedDestination.Y)
            {
                possibleMoves.Add(dy > wrapped_dy ? Direction.NORTH : Direction.SOUTH);
            }
            else if (normalizedSource.Y > normalizedDestination.Y)
            {
                possibleMoves.Add(dy < wrapped_dy ? Direction.NORTH : Direction.SOUTH);
            }

            return possibleMoves;
        }

        public Direction NaiveNavigate(Ship ship, Position destination)
        {
            // getUnsafeMoves normalizes for us
            foreach (var direction in getUnsafeMoves(ship.Position, destination))
            {
                var targetPos = ship.Position.DirectionalOffset(direction);
                if (!At(targetPos).isOccupied())
                {
                    At(targetPos).markUnsafe(ship);
                    return direction;
                }
            }

            return Direction.STILL;
        }

        internal void _update()
        {
            for (int y = 0; y < Height; ++y)
            {
                for (int x = 0; x < Width; ++x)
                {
                    Cells[x, y].Ship = null;
                }
            }

            int updateCount = Input.readInput().getInt();

            for (int i = 0; i < updateCount; ++i)
            {
                Input input = Input.readInput();
                int x = input.getInt();
                int y = input.getInt();

                Cells[x, y].Halite = input.getInt();
            }
        }

        internal static GameMap _generate()
        {
            Input mapInput = Input.readInput();
            int width = mapInput.getInt();
            int height = mapInput.getInt();

            GameMap map = new GameMap(width, height);

            for (int y = 0; y < height; ++y)
            {
                Input rowInput = Input.readInput();

                for (int x = 0; x < width; ++x)
                {
                    int halite = rowInput.getInt();
                    map.Cells[x, y] = new MapCell(new Position(x, y), halite);
                }
            }

            return map;
        }
    }
}
