package hlt

class Position(val x: Int, val y: Int) {

    internal fun directionalOffset(d: Direction): Position {
        val dx: Int
        val dy: Int

        when (d) {
            Direction.NORTH -> {
                dx = 0
                dy = -1
            }
            Direction.SOUTH -> {
                dx = 0
                dy = 1
            }
            Direction.EAST -> {
                dx = 1
                dy = 0
            }
            Direction.WEST -> {
                dx = -1
                dy = 0
            }
            Direction.STILL -> {
                dx = 0
                dy = 0
            }
        }

        return Position(x + dx, y + dy)
    }
}
