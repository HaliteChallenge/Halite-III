import hlt.*;

import java.util.Map;
import java.util.HashMap;

public class MyBot {
	public static void main(String[] args) throws java.io.IOException {

		GameParameters g = Networking.getInit();	
		GameMap map = g.getMap();
		Player[] players = g.getPlayers();
		int myID = g.getMyID();

		Networking.sendInit("JavaSP");	
		HashMap<Location, Direction> moves = new HashMap<Location, Direction>();
		while(true) {
			moves.clear();
			players = Networking.getFrame();
			for(Map.Entry<Location, Entity> entry: players[myID].getEntities().entrySet()) {
				moves.put(entry.getKey(), Direction.randomDirection());		
			}
			Networking.sendFrame(moves, players[myID].getEnergy() >= 255000 ? 255 : 0);
		}
	}
}
