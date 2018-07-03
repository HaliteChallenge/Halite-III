package hlt;

public class Location {
	public int x, y;
	public Location(int x_, int y_) {
		x = x_;
		y = y_;
	}

	public static boolean isEqual(Location o, Location t) {
		return o.x == t.x && o.y == t.y;
	}
}
