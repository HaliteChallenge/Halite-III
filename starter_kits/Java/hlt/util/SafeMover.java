package hlt.util;

import java.util.ArrayList;

import hlt.*;
import hlt.command.Command;

public class SafeMover {
    GameMap gameMap;
    Player[] players;
    ArrayList<Location> moves;

    public SafeMover(GameMap gameMap, Player[] players) {
        this.gameMap = gameMap;
        this.players = players;
        this.moves = new ArrayList<>();
    }

    public Command move(Ship ship, Direction direction, int tries) {
        if (tries <= 0) {
            return null;
        }

        Location target = gameMap.locationWithDirection(ship.getLocation(), direction);

        for (Location other : moves) {
            if (other.equals(target)) {
                return move(ship, Direction.randomDirection(), tries - 1);
            }
        }

        for (Player player : players) {
            for (Ship other : player.getShips()) {
                if (other.getLocation().equals(target)) {
                    return move(ship, Direction.randomDirection(), tries - 1);
                }
            }
        }

        moves.add(target);

        return ship.moveUnsafe(direction);
    }

    public Command move(Ship ship, Direction direction) {
        return move(ship, direction, 5);
    }

    public Command moveTowards(Ship ship, Location target) {
        return move(ship, gameMap.towards(ship.getLocation(), target));
    }
}
