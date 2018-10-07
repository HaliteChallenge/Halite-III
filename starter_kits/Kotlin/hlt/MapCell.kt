package hlt

class MapCell(val position: Position, var halite: Int) {
    var ship: Ship? = null
    var structure: Entity? = null

    val isEmpty: Boolean
        get() = ship == null && structure == null

    val isOccupied: Boolean
        get() = ship != null

    fun hasStructure(): Boolean {
        return structure != null
    }

    fun markUnsafe(ship: Ship) {
        this.ship = ship
    }
}
