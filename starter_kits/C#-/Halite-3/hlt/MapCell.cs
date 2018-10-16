using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class DictionaryCell
    {
        public Position position;
        public int halite;
        public Ship ship;
        public Entity structure;

        public DictionaryCell( Position position,  int halite)
        {
            this.position = position;
            this.halite = halite;
        }

        public bool isEmpty()
        {
            return ship == null && structure == null;
        }

        public bool isOccupied()
        {
            return ship != null;
        }

        public bool hasStructure()
        {
            return structure != null;
        }

        public void markUnsafe( Ship ship)
        {
            this.ship = ship;
        }
    }
}
