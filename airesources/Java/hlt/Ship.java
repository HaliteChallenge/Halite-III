package hlt;

import hlt.command.*;

public class Ship {
    private int id;
    private Location location;
    private int halite;

    public Ship(int id_, Location location_, int halite_) {
        id = id_;
        location = location_;
        halite = halite_;
    }

    public Command moveUnsafe(Direction direction) {
        return new Move(id, direction);
    }
    public Command dump(int amount) {
        return new Dump(id, amount);
    }
    public Command construct() {
        return new Construct(id);
    }

    public int getID() { return id; }
    public int getHalite() { return halite; }
    public Location getLocation() { return location; }
}
