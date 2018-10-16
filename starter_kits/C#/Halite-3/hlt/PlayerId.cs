using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class PlayerId
    {
        public int id;

        public PlayerId(int id)
        {
            this.id = id;
        }

        override
        public string ToString()
        {
            return id.ToString();
        }

        override
        public bool Equals(Object o)
        {
            if (this == o) return true;
            //if (o == null || getClass() != o.getClass()) return false;

            PlayerId playerId = (PlayerId)o;

            return id == playerId.id;
        }

        override
        public int GetHashCode()
        {
            return id;
        }
    }
}
