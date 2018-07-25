package hlt;

public class Ship {
	private int id;
	private Location location;
	public int halite;

	public Ship(int id_, Location location_, int halite_) {
		id = id_;
		location = location_;
		halite = halite_;
	}

	public void move(Direction direction) {
		Networking.move(id, direction);
	}
	public void dumpHalite(int haliteAmount) {
		Networking.dump(id, haliteAmount);
	}
	public void transformToShipyard() {
		Networking.construct(id);
	}

	public int getID() { return id; }
	public Location getLocation() { return location; }
}
