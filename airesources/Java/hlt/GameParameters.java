package hlt;

public class GameParameters {
	public GameMap map;
	public Player[] players;
	public int myID;

	public GameParameters(GameMap m, Player[] p, int mi) {
		map = m;
		players = p;
		myID = mi;
	}
}
