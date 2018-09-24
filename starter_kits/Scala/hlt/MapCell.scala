package hlt

class MapCell(val position: Position, var halite: Int) {
  var ship: Ship = null
  var structure: Entity = null

  def isEmpty: Boolean = ship == null && structure == null

  def isOccupied: Boolean = ship != null

  def hasStructure: Boolean = structure != null

  def markUnsafe(ship: Ship): Unit = this.ship = ship
}
