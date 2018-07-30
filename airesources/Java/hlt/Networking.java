package hlt;

import java.util.Map;
import java.util.ArrayList;
import java.io.FileWriter;
import java.io.IOException;

public class Networking {
    private static Player[] players;
    private static GameMap map;
    private static String moveBuffer = "";
    private static int myID;
    private static int turnNumber;

    private static String readLine() {
        try {
            StringBuilder builder = new StringBuilder();
            int buffer;

            for (; (buffer = System.in.read()) >= 0;) {
                if (buffer == '\n') {
                    break;
                }
                if (buffer == '\r') { // Ignore carriage return if on windows for manual testing.
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

    public static void getInit() {
        try {
            Log.initialize(new FileWriter("Log.log"));
        } catch (IOException e) {
            e.printStackTrace();
        }
        String[] c = getSplitLine();
        int numPlayers = Integer.parseInt(c[0]);
        myID = Integer.parseInt(c[1]);
        players = new Player[numPlayers];
        for(int i = 0; i < numPlayers; i++) {
            c = getSplitLine();
            Location fl = new Location(Integer.parseInt(c[1]), Integer.parseInt(c[2]));
            players[i] = new Player(Integer.parseInt(c[0]), 0, fl, new ArrayList<Ship>(), new ArrayList<Dropoff>());
        }

        c = getSplitLine();
        int width = Integer.parseInt(c[0]);
        int height = Integer.parseInt(c[1]);
        int[][] grid = new int[height][width];
        for(int y=0; y<height; y++) {
            c = getSplitLine();
            for(int x=0; x<width; x++) {
                grid[y][x] = Integer.parseInt(c[x]);
            }
        }
        map = new GameMap(width, height, grid);
    }


    public static void sendInit(String name) {
        if(name.length() < 1) System.out.println(' ');
        else System.out.println(name);
        System.out.flush();
    }
    public static void getFrame() {
        // TODO give this out
        turnNumber = Integer.parseInt(readLine());
        String[] c;
        for(int i=0; i<players.length; i++) {
            c = getSplitLine();
            int playerID = Integer.parseInt(c[0]);
            int numShips = Integer.parseInt(c[1]);
            int numDropoffs = Integer.parseInt(c[2]);
            int halite = Integer.parseInt(c[3]);
            ArrayList<Ship> ships = new ArrayList<Ship>();
            ArrayList<Dropoff> dropoffs = new ArrayList<Dropoff>();
            for(int j=0; j<numShips; j++) {
                c = getSplitLine();
                ships.add(new Ship(
                    Integer.parseInt(c[0]),
                    new Location(Integer.parseInt(c[1]), Integer.parseInt(c[2])),
                    Integer.parseInt(c[3])));
            }
            for(int j=0; j<numDropoffs; j++) {
                c = getSplitLine();
                dropoffs.add(new Dropoff(
                    Integer.parseInt(c[0]),
                    new Location(Integer.parseInt(c[1]), Integer.parseInt(c[2]))));
            }
            players[playerID] = new Player(playerID,
                halite,
                players[i].getShipyardLocation(),
                ships,
                dropoffs);
        }
        int numChangedCells = Integer.parseInt(readLine());
        for(int i=0; i<numChangedCells; i++) {
            c = getSplitLine();
            map.setSquare(Integer.parseInt(c[0]), Integer.parseInt(c[1]), Integer.parseInt(c[2]));
        }
    }
    public static void sendFrame() {
        System.out.println(moveBuffer);
        System.out.flush();
        moveBuffer = "";
    }

    public static void move(int shipID, Direction direction) {
        if(direction != Direction.STILL) moveBuffer += "m " + shipID + " " + direction.getCharRepresent() + " ";
    }
    public static void dump(int shipID, int haliteAmount) {
        moveBuffer += "d " + shipID + " " + haliteAmount + " ";
    }
    public static void construct(int shipID) {
        moveBuffer += "c " + shipID + " ";
    }
    public static void spawn(int halite) {
        moveBuffer += "g " + halite + " ";
    }
    public static int getTurnNumber() { return turnNumber; }
    public static int getMyID() { return myID; }
    public static GameMap getMap() { return map; }
    public static Player[] getPlayers() { return players; }
}
