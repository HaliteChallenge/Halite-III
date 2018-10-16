using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class Position
    {
        public int x;
        public int y;

        public Position(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        public Position directionalOffset(Direction d)
        {
            int dx;
            int dy;

            if (d is NORTH)
            {
                dx = 0;
                dy = -1;
            }
            else if (d is SOUTH)
            {
                dx = 0;
                dy = 1;
            }
            else if (d is EAST)
            {
                dx = 1;
                dy = 0;
            }
            else if (d is WEST)
            {
                dx = -1;
                dy = 0;
            }
            else if (d is STILL)
            {
                dx = 0;
                dy = 0;
            }
            else
            {
                throw new Exception("Unknown direction " + d);
            }

            return new Position(x + dx, y + dy);
        }

        override
        public bool Equals(Object o)
        {
            if (this == o) return true;
            //if (o == null || getClass() != o.getClass()) return false;

            Position position = (Position)o;

            if (x != position.x) return false;
            return y == position.y;
        }

        override
        public int GetHashCode()
        {
            int result = x;
            result = 31 * result + y;
            return result;
        }
    }
}
