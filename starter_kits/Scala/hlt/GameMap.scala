package hlt

object GameMap {
  private[hlt] def _generate: GameMap = {
    val mapInput = Input.readInput
    val width = mapInput.getInt
    val height = mapInput.getInt
    val map = new GameMap(width, height)
    for (y <- 0 until height) {
      val rowInput = Input.readInput
      for (x <- 0 until width) {
        val halite = rowInput.getInt
        map.cells(y)(x) = new MapCell(new Position(x, y), halite)
      }
    }
    map
  }
}

class GameMap(val width: Int, val height: Int) {
  var cells: Array[Array[MapCell]] = Array.ofDim[MapCell](height, width)

  def at(entity: Entity): MapCell = at(entity.position)

  def calculateDistance(source: Position, target: Position): Int = {
    def calculate(normalizedSource: Position, normalizedTarget: Position): Int = {
      val dx = Math.abs(normalizedSource.x - normalizedTarget.x)
      val dy = Math.abs(normalizedSource.y - normalizedTarget.y)
      val toroidal_dx = Math.min(dx, width - dx)
      val toroidal_dy = Math.min(dy, height - dy)
      toroidal_dx + toroidal_dy
    }
    calculate(normalize(source), normalize(target))
  }

  def normalize(position: Position) = {
    new Position(
      ((position.x % width) + width) % width, ((position.y % height) + height) % height)
  }

  def naiveNavigate(ship: Ship, destination: Position): Direction =  {
    def navigate(normalizedDestination: Position): Direction = {
      for (direction <- getUnsafeMoves(ship.position, normalizedDestination)) {
        val targetPos = ship.position.directionalOffset(direction)
        if (!at(targetPos).isOccupied) {
          at(targetPos).markUnsafe(ship)
          return direction
        }
      }
      Direction.STILL
    }
    navigate(normalize(destination))
  }

  def at(position: Position): MapCell = cells(position.y)(position.x)

  def getUnsafeMoves(source: Position, destination: Position): List[Direction] = {
    val dx = Math.abs(source.x - destination.x)
    val dy = Math.abs(source.y - destination.y)
    val wrapped_dx = width - dx
    val wrapped_dy = height - dy
    val xDirection = if (source.x < destination.x) {
      if (dx > wrapped_dx) Direction.WEST
      else
        Direction.EAST
    }
    else if (source.x > destination.x) {
      if (dx < wrapped_dx)
        Direction.WEST
      else
        Direction.EAST
    }
    else
      Direction.STILL
    val yDirection = if (source.y < destination.y) {
      if (dy > wrapped_dy)
        Direction.NORTH
      else
        Direction.SOUTH
    }
    else if (source.y > destination.y) {
      if (dy < wrapped_dy)
        Direction.NORTH
      else
        Direction.SOUTH
    }
    else
      Direction.STILL
    List(xDirection, yDirection)
  }

  def _update() = {
    cells.foreach(row =>
      row.foreach(cell => cell.ship = null))
    val updateCount = Input.readInput.getInt
    (0 until updateCount).foreach { _ =>
      val input = Input.readInput
      val x = input.getInt
      val y = input.getInt
      cells(y)(x).halite = input.getInt
    }
  }
}
