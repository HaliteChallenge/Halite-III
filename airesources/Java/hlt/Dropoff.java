package hlt;

public class Dropoff {
    private int id;
    private Location location;

    public Dropoff(int id_, Location location_) {
        id = id_;
        location = location_;
    }

    public int getID() { return id; }
    public Location getLocation() { return location; }
}
