import hlt.*;

import java.util.Map;
import java.util.HashMap;

public class MyBot {
	public static void main(String[] args) throws java.io.IOException {
		// Get starting info
		Networking.getInit();
		GameMap map = Networking.getMap();
		Player[] players = Networking.getPlayers();
		int myID = Networking.getMyID();

		// Send back the name of our bot
		Networking.sendInit("MyJavaBot-" + myID);
		while (true) {
			// Get frame info
			Networking.getFrame();
			players = Networking.getPlayers();
			map = Networking.getMap();

			// Dump halite at our shipyard if a ship is sitting on top of it
			// Otherwise, move our ships randomnly
			for (Ship ship: players[myID].getShips()) {
				if (ship.getLocation() == players[myID].getShipyardLocation()
						&& ship.halite > Constants.MAX_HALITE / 4) {
					ship.dumpHalite(ship.halite);
				} else if (map.getHalite(ship.getLocation()) < Constants.MAX_HALITE / 10) {
					ship.move(Direction.randomDirection());
				}
			}

			// Spawn a ship if we're in the start or middle of the game
			// and if we have the available halite
			if (Networking.getTurnNumber() <= 200 && players[myID].getHalite() >= Constants.SHIP_COST)  {
				Networking.spawn(0);
			}

			//Send back our commands
			Networking.sendFrame();
		}
	}
}
