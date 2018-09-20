package hlt

class Position(val x: Int, val y: Int) {
  def directionalOffset(d: Direction) = {
    val (dx, dy) = d match {
      case Direction.NORTH => (0, -1)
      case Direction.SOUTH => (0, 1)
      case Direction.EAST => (1, 0)
      case Direction.WEST => (-1, 0)
      case Direction.STILL => (0, 0)
      case _ =>
        throw new IllegalStateException(s"Unknown direction '$d'")
    }
    new Position(x + dx, y + dy)
  }

  override def toString = s"Position{x=$x,y=$y}"
}
