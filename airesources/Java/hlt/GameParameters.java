package hlt;

public class GameParameters {
	private GameMap map;
	private Player[] players;
	private int myID;

	public GameParameters(GameMap m, Player[] p, int mi) {
		map = m;
		players = p;
		myID = mi;
	}
	public GameMap getMap() { return map; }
	public Player[] getPlayers() { return players; }
	public int getMyID() { return myID; }

}
