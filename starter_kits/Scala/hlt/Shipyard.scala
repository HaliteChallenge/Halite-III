package hlt

class Shipyard(owner: PlayerId, position: Position) extends Entity(owner, EntityId.NONE, position) {
  def spawn: Command = Command.spawnShip

  override def toString = s"Shipyard{owner=${owner.id},position=$position}"
}
