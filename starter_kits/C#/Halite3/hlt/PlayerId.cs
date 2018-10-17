using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Halite_Project.hlt {
    public class PlayerId {
        public int id;

        public PlayerId(int id) {
            this.id = id;
        }

        public override string ToString() {
            return id.ToString();
        }

        public override bool Equals(object obj) {
            if (this == obj) return true;
            if (obj == null || GetType() != obj.GetType()) return false;

            PlayerId playerId = (PlayerId)obj;

            return id == playerId.id;
        }

        public override int GetHashCode() {
            return id;
        }
    }
}
