package hlt

import java.util.*

class GameMap(private val width: Int, private val height: Int) {
    val cells: Array<Array<MapCell?>?> = arrayOfNulls<Array<MapCell?>?>(height)

    init {
        for (y in 0 until height) {
            cells[y] = arrayOfNulls<MapCell?>(width)
        }
    }

    fun at(position: Position): MapCell? {
        val normalized = normalize(position)
        return cells[normalized.y]?.get(normalized.x)
    }

    fun at(entity: Entity): MapCell? {
        return at(entity.position)
    }

    fun calculateDistance(source: Position, target: Position): Int {
        val normalizedSource = normalize(source)
        val normalizedTarget = normalize(target)

        val dx = Math.abs(normalizedSource.x - normalizedTarget.x)
        val dy = Math.abs(normalizedSource.y - normalizedTarget.y)

        val toroidal_dx = Math.min(dx, width - dx)
        val toroidal_dy = Math.min(dy, height - dy)

        return toroidal_dx + toroidal_dy
    }

    fun normalize(position: Position): Position {
        val x = (position.x % width + width) % width
        val y = (position.y % height + height) % height
        return Position(x, y)
    }

    fun getUnsafeMoves(source: Position, destination: Position): ArrayList<Direction> {
        val possibleMoves = ArrayList<Direction>()

        val normalizedSource = normalize(source)
        val normalizedDestination = normalize(destination)

        val dx = Math.abs(normalizedSource.x - normalizedDestination.x)
        val dy = Math.abs(normalizedSource.y - normalizedDestination.y)
        val wrappedDx = width - dx
        val wrappedDy = height - dy

        if (normalizedSource.x < normalizedDestination.x) {
            possibleMoves.add(if (dx > wrappedDx) Direction.WEST else Direction.EAST)
        } else if (normalizedSource.x > normalizedDestination.x) {
            possibleMoves.add(if (dx < wrappedDx) Direction.WEST else Direction.EAST)
        }

        if (normalizedSource.y < normalizedDestination.y) {
            possibleMoves.add(if (dy > wrappedDy) Direction.NORTH else Direction.SOUTH)
        } else if (normalizedSource.y > normalizedDestination.y) {
            possibleMoves.add(if (dy < wrappedDy) Direction.NORTH else Direction.SOUTH)
        }

        return possibleMoves
    }

    fun naiveNavigate(ship: Ship, destination: Position): Direction {
        // getUnsafeMoves normalizes for us
        for (direction in getUnsafeMoves(ship.position, destination)) {
            val targetPos = ship.position.directionalOffset(direction)
            if (at(targetPos)?.isOccupied != true) {
                at(targetPos)?.markUnsafe(ship)
                return direction
            }
        }

        return Direction.STILL
    }

    internal fun _update() {
        for (y in 0 until height) {
            for (x in 0 until width) {
                cells[y]?.get(x)?.ship = null
            }
        }

        val updateCount = Input.readInput().nextInt

        for (i in 0 until updateCount) {
            val input = Input.readInput()
            val x = input.nextInt
            val y = input.nextInt

            cells[y]?.get(x)?.halite = input.nextInt
        }
    }

    companion object {

        internal fun _generate(): GameMap {
            val mapInput = Input.readInput()
            val width = mapInput.nextInt
            val height = mapInput.nextInt

            val map = GameMap(width, height)

            for (y in 0 until height) {
                val rowInput = Input.readInput()

                for (x in 0 until width) {
                    val halite = rowInput.nextInt
                    map.cells[y]?.set(x, MapCell(Position(x, y), halite))
                }
            }

            return map
        }
    }
}