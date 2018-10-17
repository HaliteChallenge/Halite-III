namespace Halite_Project.hlt
{
    public class Position{
        public int x;
        public int y;

        public Position(int x, int y){
            this.x = x;
            this.y = y;
        }

        public Position DirectionalOffset(EDirection d){
            int dx;
            int dy;

            switch (d)
            {
                case EDirection.NORTH:
                    dx = 0;
                    dy = -1;
                    break;
                case EDirection.SOUTH:
                    dx = 0;
                    dy = 1;
                    break;
                case EDirection.EAST:
                    dx = 1;
                    dy = 0;
                    break;
                case EDirection.WEST:
                    dx = -1;
                    dy = 0;
                    break;
                case EDirection.STILL:
                    dx = 0;
                    dy = 0;
                    break;
                default:
                    throw new System.InvalidOperationException("Unknown direction " + d);
            }

            return new Position(x + dx, y + dy);
        }

        public override bool Equals(object o){
            if (this == o) return true;
            if (o == null || !this.GetType().Equals(o.GetType())) return false;

            Position position = (Position)o;

            if (x != position.x) return false;
            return y == position.y;
        }

        public override int GetHashCode(){
            int result = x;
            result = 31 * result + y;
            return result;
        }
    }
}