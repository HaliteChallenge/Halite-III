package hlt;

import hlt.command.*;

public class Shipyard extends Dropoff {
    public Shipyard(Location location) {
        super(-1, location);
    }

    public Command spawn(int initialHalite) {
        return new Spawn(initialHalite);
    }
}
