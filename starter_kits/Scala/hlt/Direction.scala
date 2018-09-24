package hlt

object Direction {

  val ALL_CARDINALS: List[Direction] = List(NORTH, EAST, SOUTH, WEST, STILL)

  object NORTH extends Direction('n')

  object EAST extends Direction('e')

  object SOUTH extends Direction('s')

  object WEST extends Direction('w')

  object STILL extends Direction('o')

}

case class Direction(charValue: Char) {
  def ~(that: Direction): Direction = {
    that match {
      case Direction.NORTH =>
        return Direction.SOUTH
      case Direction.EAST =>
        return Direction.WEST
      case Direction.SOUTH =>
        return Direction.NORTH
      case Direction.WEST =>
        return Direction.EAST
      case Direction.STILL =>
        return Direction.STILL
      case _ =>
        throw new IllegalStateException("Unknown direction " + this)
    }
  }
}
