package hlt

object Command {
  def spawnShip = new Command("g")

  def transformShipIntoDropoffSite(id: EntityId) = new Command(s"c ${id.id}")

  def move(id: EntityId, direction: Direction) = new Command(s"m ${id.id} ${direction.charValue}")
}

class Command private(val command: String) {
  override def toString: String = s"{Command=$command}"
}
