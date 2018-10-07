package hlt

import java.util.*

class Player private constructor(val id: PlayerId, val shipyard: Shipyard) {
    var halite: Int = 0
    val ships: MutableMap<EntityId, Ship> = LinkedHashMap()
    val dropoffs: MutableMap<EntityId, DropOff> = LinkedHashMap()

    internal fun _update(numShips: Int, numDropoffs: Int, halite: Int) {
        this.halite = halite

        ships.clear()
        for (i in 0 until numShips) {
            val ship = Ship._generate(id)
            ships.put(ship.id, ship)
        }

        dropoffs.clear()
        for (i in 0 until numDropoffs) {
            val dropoff = DropOff.generate(id)
            dropoffs.put(dropoff.id, dropoff)
        }
    }

    companion object {

        internal fun _generate(): Player {
            val input = Input.readInput()

            val playerId = PlayerId(input.nextInt)
            val shipyard_x = input.nextInt
            val shipyard_y = input.nextInt

            return Player(playerId, Shipyard(playerId, Position(shipyard_x, shipyard_y)))
        }
    }
}
