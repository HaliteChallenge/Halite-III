package hlt;

import java.util.ArrayList;
import java.lang.Exception;

public class Player {
    private int playerID;
    private int halite;
    private Shipyard shipyard;
    private ArrayList<Ship> ships;
    private ArrayList<Dropoff> dropoffs;

    public Player(int playerID_, int halite_, Shipyard shipyard_, ArrayList<Ship> ships_, ArrayList<Dropoff> dropoffs_) {
        playerID = playerID_;
        halite = halite_;
        shipyard = shipyard_;
        ships = ships_;
        dropoffs = dropoffs_;
    }

    public int getPlayerID() { return playerID; }
    public int getHalite() { return halite; }
    public Shipyard getShipyard() { return shipyard; }
    public ArrayList<Ship> getShips() { return ships; }
    public ArrayList<Dropoff> getDropoffs() { return dropoffs; }

    public void addShip(Ship ship) throws Exception {
        if(ships.contains(ship)) throw new Exception("Tried to add a duplicate ship to a player!");
        else ships.add(ship);
    }
    public void addDropoff(Dropoff dropoff) throws Exception {
        if(dropoffs.contains(dropoff)) throw new Exception("Tried to add a duplicate dropoff to a player!");
        else dropoffs.add(dropoff);
    }
}
