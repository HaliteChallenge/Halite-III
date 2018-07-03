package hlt;

import java.util.Map;
import java.util.HashMap;

public class Networking {
	static Player[] lastPlayers;

	private static String readLine() {
        try {
            StringBuilder builder = new StringBuilder();
            int buffer;

            for (; (buffer = System.in.read()) >= 0;) {
                if (buffer == '\n') {
                    break;
                }
                if (buffer == '\r') {
                    // Ignore carriage return if on windows for manual testing.
                    continue;
                }
                builder = builder.append((char)buffer);
            }
            return builder.toString();
        } catch(final Exception e) {
            System.exit(1);
            throw new RuntimeException(e);
        }
    }
	
	private static String[] getSplitLine() { return readLine().split(" "); }

	public static GameParameters getInit() {
		String[] c = getSplitLine();		
		int numPlayers = Integer.parseInt(c[0]);
		int myID = Integer.parseInt(c[1]);
		Player[] players = new Player[numPlayers];
		for(int i = 0; i < numPlayers; i++) {
			c = getSplitLine();
			Location fl = new Location(Integer.parseInt(c[1]), Integer.parseInt(c[2]));
			players[i] = new Player(Integer.parseInt(c[0]), 0, fl, new HashMap<Location, Entity>());
		}
		
		c = getSplitLine();
		int width = Integer.parseInt(c[0]);
		int height = Integer.parseInt(c[1]);
		Cell[][] grid = new Cell[height][width];
		for(int y=0; y<height; y++) {
			for(int x=0; x<width; x++) {
				c = getSplitLine();
				if(c[0].equals("f")) {
					grid[y][x] = new Cell(0, true, 1);	
				} else {
					grid[y][x] = new Cell(Integer.parseInt(c[1]), true, 1);	
				} 
			}
		}
		GameMap map = new GameMap(width, height, grid);
		
		lastPlayers = players;
		return new GameParameters(map, players, myID);
	}


	public static void sendInit(String name) {
		if(name.length() < 1) System.out.println(' ');
		else System.out.println(name);
		System.out.flush();
	}
	public static Player[] getFrame() {
		int turnNumber = Integer.parseInt(readLine());
		String[] c;
		for(int i=0; i<lastPlayers.length; i++) {
			c = getSplitLine();	
			int playerID = Integer.parseInt(c[0]);
			int numEntities = Integer.parseInt(c[1]);
			int storedEnergy = Integer.parseInt(c[2]);
			HashMap<Location, Entity> entities = new HashMap<Location, Entity>();
			for(int j=0; j<numEntities; j++) {
				c = getSplitLine();	
				entities.put(
					new Location(Integer.parseInt(c[0]), Integer.parseInt(c[1])), 
					new Entity(playerID, Integer.parseInt(c[3])));
			}
			lastPlayers[playerID] = new Player(playerID, 
					storedEnergy, 
					lastPlayers[i].getFactoryLocation(), 
					entities);
		}
		return lastPlayers;
	}
	public static void sendFrame(HashMap<Location, Direction> moves) {
		for(Map.Entry<Location, Direction> entry: moves.entrySet()) {
			if(entry.getValue() == Direction.STILL) continue;
			System.out.print("m ");
			System.out.print(entry.getKey().x);
			System.out.print(' ');
			System.out.print(entry.getKey().y);
			System.out.print(' ');
			System.out.print(entry.getValue().getCharRepresent());
			System.out.print(' ');
		}
		System.out.print('\n');
		System.out.flush();
	}
}
