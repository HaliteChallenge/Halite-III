import hlt._
import java.util.Random

object MyBotScala {
  def main(args: Array[String]) = {
    val rngSeed = if (args.length > 1) args(1).toInt else System.nanoTime
    val rng = new Random(rngSeed)
    val game = new Game("MyScalaBot")
    Log.log(s"Successfully created bot! My Player ID is $game.myId. Bot rng seed is $rngSeed.")
    while (true) {
      game.updateFrame()
      val me = game.me
      val gameMap = game.gameMap
      val commands: List[Command] = me.ships.values.map({
        ship => if (gameMap.at(ship).halite < Constants.MAX_HALITE / 10 || ship.isFull)
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