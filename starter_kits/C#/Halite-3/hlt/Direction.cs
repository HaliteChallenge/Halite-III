using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    public class Direction
    {
        //NORTH('n'),
        //EAST('e'),
        //SOUTH('s'),
        //WEST('w'),
        //STILL('o')

        public char charValue;

        public static List<Direction> ALL_CARDINALS = new List<Direction>{ new NORTH(), new SOUTH(), new EAST(), new WEST() };

        public Direction invertDirection()
        {
            if (this is NORTH)
            {
                return new SOUTH();
            }
            else if (this is SOUTH)
            {
                return new NORTH();
            }
            else if (this is EAST)
            {
                return new WEST();
            }
            else if (this is WEST)
            {
                return new EAST();
            }
            else if (this is STILL)
            {
                return new STILL();
            }
            else
            {
                throw new Exception("Unknown direction " + this);
            }
        }

        protected Direction (char charValue)
        {
            this.charValue = charValue;
        }

    }

    public class NORTH : Direction
    {
        public NORTH()
			:base('n')
        {

        }
    }

    public class SOUTH : Direction
    {
        public SOUTH()
            : base('s')
        {

        }
    }
    public class EAST : Direction
    {
        public EAST()
            : base('e')
        {

        }
    }

    public class WEST : Direction
    {
        public WEST()
            : base('w')
        {

        }
    }

    public class STILL : Direction
    {
        public STILL()
               : base('o')
        {

        }
    }
}
