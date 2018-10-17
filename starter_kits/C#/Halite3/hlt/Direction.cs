
using System;
using System.Collections.Generic;

namespace Halite_Project.hlt {
    public enum EDirection {
        NORTH = 'n',
        EAST = 'e',
        SOUTH = 's',
        WEST = 'w',
        STILL = 'o'
    }

    public class Direction {
        public EDirection direction = EDirection.STILL;

        public static List<EDirection> ALL_CARDINALS = new List<EDirection>();

        static Direction() {
            ALL_CARDINALS.Add(EDirection.NORTH);
            ALL_CARDINALS.Add(EDirection.SOUTH);
            ALL_CARDINALS.Add(EDirection.EAST);
            ALL_CARDINALS.Add(EDirection.WEST);
        }

        public EDirection InvertDirection() {
            switch (direction) {
                case EDirection.NORTH: return EDirection.SOUTH;
                case EDirection.EAST: return EDirection.WEST;
                case EDirection.SOUTH: return EDirection.NORTH;
                case EDirection.WEST: return EDirection.EAST;
                case EDirection.STILL: return EDirection.STILL;
                default: throw new Exception("Unknown direction " + direction);
            }
        }

        private Direction(EDirection direction) {
            this.direction = direction;
        }
    }
}