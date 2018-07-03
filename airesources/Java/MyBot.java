import hlt.*;

import java.util.Map;
import java.util.HashMap;

public class MyBot {
	public static void main(String[] args) throws java.io.IOException {
		GameParameters g = Networking.getInit();	
		GameMap map = g.map;
		Player[] players = g.players;
		int myID = g.myID;

		Networking.sendInit("JavaSP");	
		HashMap<Location, Direction> moves = new HashMap<Location, Direction>();
		while(true) {
			moves.clear();
			players = Networking.getFrame();
			for(Map.Entry<Location, Entity> entry: players[myID].entities.entrySet()) {
				moves.put(entry.getKey(), Direction.randomDirection());		
			}
			Networking.sendFrame(moves);
		}
	}
}
