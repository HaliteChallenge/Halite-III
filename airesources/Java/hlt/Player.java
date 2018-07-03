package hlt;

import java.util.HashMap;

public class Player {
	public int playerID;
	public int energy;
	public Location factoryLocation;
	public HashMap<Location, Entity> entities;
	
	public Player(int p, int e, Location f, HashMap<Location, Entity> ent) {
		playerID = p;
		energy = e;
		factoryLocation = f;
		entities = ent;
	}

	// Place entity in our entities hashmap
	// Accounts for merging of entities
	public void addEntity(Location l, Entity e) {
		if(entities.containsKey(l)) entities.get(l).energy += e.energy;	
		else entities.put(l, e);
	}

}
