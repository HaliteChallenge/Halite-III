package hlt;

import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Map;

import hlt.command.Command;

public class Game {
    private Player[] players;
    private GameMap map;
    private int myID;
    private int turnNumber;

    public Game() {
        try {
            Log.initialize(new FileWriter("Log.log"));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private String readLine() {
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

    private String[] getSplitLine() { return readLine().split(" "); }

    public void getInit() {
        String[] c = getSplitLine();
        int numPlayers = Integer.parseInt(c[0]);
        myID = Integer.parseInt(c[1]);
        players = new Player[numPlayers];
        for(int i = 0; i < numPlayers; i++) {
            c = getSplitLine();
            Shipyard fl = new Shipyard(new Location(Integer.parseInt(c[1]), Integer.parseInt(c[2])));
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


    public void sendInit(String name) {
        if(name.length() < 1) System.out.println(' ');
        else System.out.println(name);
        System.out.flush();
    }

    public int getFrame() {
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
                                           players[i].getShipyard(),
                                           ships,
                                           dropoffs);
        }

        int numChangedCells = Integer.parseInt(readLine());
        for(int i=0; i<numChangedCells; i++) {
            c = getSplitLine();
            map.setSquare(Integer.parseInt(c[0]), Integer.parseInt(c[1]), Integer.parseInt(c[2]));
        }

        return turnNumber;
    }

    public void endTurn(ArrayList<Command> commands) {
        StringBuilder moveBuffer = new StringBuilder();
        for (Command command : commands) {
            if (command == null) {
                continue;
            }
            command.build(moveBuffer);
            moveBuffer.append(' ');
        }
        System.out.println(moveBuffer);
        System.out.flush();
    }

    public int getTurnNumber() { return turnNumber; }
    public int getMyID() { return myID; }
    public Player getMe() { return players[myID]; }
    public GameMap getMap() { return map; }
    public Player[] getPlayers() { return players; }
}
