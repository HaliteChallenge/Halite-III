import java.util.Random

import hlt._

object MyBot {
  def main(args: Array[String]) = {
    val rngSeed = if (args.length > 1) args(1).toInt else System.nanoTime
    val rng = new Random(rngSeed)

    val game = new Game()
    // At this point "game" variable is populated with initial map data.
    // This is a good place to do computationally expensive start-up pre-processing.
    // As soon as you call "ready" function below, the 2 second per turn timer will start.
    game.ready("MyBot")

    Log.log(s"Successfully created bot! My Player ID is $game.myId. Bot rng seed is $rngSeed.")

    while (true) {
      game.updateFrame()
      val me = game.me
      val gameMap = game.gameMap
      val commands: List[Command] = me.ships.values.map({
        ship =>
          if (gameMap.at(ship).halite < Constants.MAX_HALITE / 10 || ship.isFull)
            ship.move(Direction.ALL_CARDINALS(rng.nextInt(4)))
          else
            ship.stayStill
      }).toList
      if (game.turnNumber <= 200 && me.halite >= Constants.SHIP_COST && !gameMap.at(me.shipyard).isOccupied)
        game.endTurn(me.shipyard.spawn :: commands)
      else
        game.endTurn(commands)
    }
  }
}
