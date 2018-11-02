using System;
using System.Collections.Generic;
using System.Text;

namespace Halite3.hlt
{
    /// <summary>
    /// A PlayerId is the identifier that corresponds to a Player.
    /// </summary>
    public class PlayerId
    {
        public readonly int id;

        public PlayerId(int id)
        {
            this.id = id;
        }

        public override string ToString()
        {
            return id.ToString();
        }

        public override bool Equals(object obj)
        {
            if (this == obj)
                return true;
            if (obj == null || this.GetType() != obj.GetType())
                return false;

            PlayerId playerId = (PlayerId)obj;
            return id == playerId.id;
        }

        public override int GetHashCode()
        {
            return id;
        }
    }
}
