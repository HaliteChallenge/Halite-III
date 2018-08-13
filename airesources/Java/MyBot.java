import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

import hlt.*;
import hlt.util.SafeMover;

public class MyBot {
    public static void main(String[] args) throws java.io.IOException {
        Game game = new Game();
        game.getInit();
        game.sendInit("MyJavaBot-" + game.getMyID());

        while (true) {
            // Get frame info
            int turnNumber = game.getFrame();

            Player me = game.getMe();
            ArrayList<hlt.command.Command> commandQueue = new ArrayList<>();
            SafeMover safeMover = new SafeMover(game.getMap(), game.getPlayers());

            // Dump halite at our shipyard if a ship is sitting on top of it
            // Otherwise, move our ships randomly
            boolean atHome = false;
            for (Ship ship: me.getShips()) {
                if (ship.getLocation().equals(me.getShipyard().getLocation())) {
                    atHome = true;
                }

                if (game.getMap().getHalite(ship.getLocation()) < Constants.MAX_HALITE / 10) {
                    commandQueue.add(safeMover.move(ship, Direction.randomDirection()));
                }
            }

            // Spawn a ship if we're in the start or middle of the game
            // and if we have the available halite
            if (turnNumber <= 200 && me.getHalite() >= Constants.SHIP_COST && !atHome)  {
                commandQueue.add(me.getShipyard().spawn());
            }

            //Send back our commands
            game.endTurn(commandQueue);
        }
    }
}
