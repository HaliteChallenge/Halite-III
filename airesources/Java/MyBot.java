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
		while(true) {
			players = Networking.getFrame();
			for(Ship ship: players[myID].getShips()) {
				if(ship.getLocation() == players[myID].getShipyardLocation() 
						&& ship.halite > Constants.MAX_HALITE / 4) {
					ship.dumpHalite(ship.halite);
				} else if(map.getHalite(ship.getLocation()) < Constants.MAX_HALITE / 10) {
					ship.move(Direction.randomDirection());
				}
			}
			if(Networking.getTurnNumber() <= 200 && players[myID].getHalite() >= Constants.SHIP_COST)  {
				Networking.spawn(0);
			}
			Networking.sendFrame();
		}
	}
}
