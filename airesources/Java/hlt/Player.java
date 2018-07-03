package hlt;

import java.util.HashMap;

public class Player {
	private int playerID;
	private int energy;
	private Location factoryLocation;
	private HashMap<Location, Entity> entities;
	
	public Player(int p, int e, Location f, HashMap<Location, Entity> ent) {
		playerID = p;
		energy = e;
		factoryLocation = f;
		entities = ent;
	}

	public int getPlayerID() { return playerID; }
	public int getEnergy() { return energy; }
	public Location getFactoryLocation() { return factoryLocation; }
	public HashMap<Location, Entity> getEntities() { return entities; }

	// Place entity in our entities hashmap
	// Accounts for merging of entities
	public void addEntity(Location l, Entity e) {
		if(entities.containsKey(l)) entities.get(l).energy += e.energy;	
		else entities.put(l, e);
	}

}
