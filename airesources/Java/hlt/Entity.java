package hlt;

public class Entity {
	private int ownerID;
	public int energy;

	public Entity(int o, int e) {
		ownerID = o;
		energy = e;
	}

	public int getOwnerID() { return ownerID; }
}
