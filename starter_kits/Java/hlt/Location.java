package hlt;

public class Location {
    public int x, y;
    public Location(int x_, int y_) {
        x = x_;
        y = y_;
    }

    public Location clone() {
        return new Location(x, y);
    }

    @Override
    public boolean equals(Object t) {
        if (t == null) {
            return false;
        }

        if (!(t instanceof Location)) {
            return false;
        }

        Location other = (Location) t;

        return x == other.x && y == other.y;
    }
}
