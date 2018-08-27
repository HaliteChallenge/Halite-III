package hlt;

import hlt.command.*;

public class Shipyard extends Dropoff {
    public Shipyard(Location location) {
        super(-1, location);
    }

    public Command spawn() {
        return new Spawn();
    }
}
