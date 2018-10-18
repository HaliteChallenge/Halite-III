package hlt

object Ship {
  def _generate(playerId: PlayerId) = {
    val input = Input.readInput
    val shipId = new EntityId(input.getInt)
    val x = input.getInt
    val y = input.getInt
    val halite = input.getInt
    new Ship(playerId, shipId, new Position(x, y), halite)
  }
}

class Ship(owner: PlayerId, id: EntityId, position: Position, val halite: Int) extends Entity(owner, id, position) {
  def isFull: Boolean = halite >= Constants.MAX_HALITE

  def makeDropoff: Command = Command.transformShipIntoDropoffSite(id)

  def move(direction: Direction): Command = Command.move(id, direction)

  def stayStill: Command = Command.move(id, Direction.STILL)

}
