package hlt

import scala.collection.mutable

object Player {
  def initialize: Player = {
    val input = Input.readInput
    val playerId = new PlayerId(input.getInt)
    val shipyard_x = input.getInt
    val shipyard_y = input.getInt
    new Player(playerId, new Shipyard(playerId, new Position(shipyard_x, shipyard_y)))
  }
}

class Player private(val id: PlayerId, val shipyard: Shipyard) {
  final val ships = new mutable.LinkedHashMap[EntityId, Ship]
  final val dropoffs = new mutable.LinkedHashMap[EntityId, Dropoff]
  var halite = 0

  def _update(numShips: Int, numDropoffs: Int, halite: Int) = {
    this.halite = halite
    ships.clear()
    (0 until numShips).foreach({ i =>
      val ship = Ship._generate(id)
      ships.put(ship.id, ship)
    })
    dropoffs.clear()
    (0 until numDropoffs).foreach({ i =>
      val dropoff = Dropoff._generate(id)
      dropoffs.put(dropoff.id, dropoff)
    })
  }

  override def toString() = s"Player{id=${id.id},shipyard=${shipyard},halite=$halite,#ships=${ships.size},#dropoffs=${dropoffs.size}}"
}
