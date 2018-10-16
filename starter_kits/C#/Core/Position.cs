using System;

namespace Halite3.Core
{
    public class Position
    {
        public int X { get; private set; }
        public int Y { get; private set; }

        public Position(int x, int y)
        {
            this.X = x;
            this.Y = y;
        }

        public Position DirectionalOffset(Direction d)
        {
            int dx;
            int dy;

            switch (d)
            {
                case Direction.NORTH:
                    dx = 0;
                    dy = -1;
                    break;
                case Direction.SOUTH:
                    dx = 0;
                    dy = 1;
                    break;
                case Direction.EAST:
                    dx = 1;
                    dy = 0;
                    break;
                case Direction.WEST:
                    dx = -1;
                    dy = 0;
                    break;
                case Direction.STILL:
                    dx = 0;
                    dy = 0;
                    break;
                default:
                    throw new ArgumentException("Unknown direction " + d);
            }

            return new Position(X + dx, Y + dy);
        }

        public override bool Equals(object other)
        {
            if (this == other) return true;
            if (!(other is Position)) return false;

            var position = other as Position;

            if (X != position.X) return false;
            return Y == position.Y;
        }

        public override int GetHashCode()
        {
            int result = X;
            result = 31 * result + Y;
            return result;
        }
    }
}
