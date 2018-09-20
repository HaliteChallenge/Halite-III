package hlt

object Dropoff {
  def _generate(playerId: PlayerId) = {
    val input = Input.readInput
    val dropoffId = new EntityId(input.getInt)
    val x = input.getInt
    val y = input.getInt
    new Dropoff(playerId, dropoffId, new Position(x, y))
  }
}

class Dropoff(owner: PlayerId, id: EntityId, position: Position) extends Entity(owner, id, position)
