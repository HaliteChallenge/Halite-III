package hlt

import java.util.*

enum class Direction constructor(val charValue: Char) {
    NORTH('n'),
    EAST('e'),
    SOUTH('s'),
    WEST('w'),
    STILL('o');

    fun invertDirection(): Direction {
        when (this) {
            NORTH -> return SOUTH
            EAST -> return WEST
            SOUTH -> return NORTH
            WEST -> return EAST
            STILL -> return STILL
            else -> throw IllegalStateException("Unknown direction " + this)
        }
    }

    companion object {

        val ALL_CARDINALS: ArrayList<Direction> = ArrayList()

        init {
            ALL_CARDINALS.add(NORTH)
            ALL_CARDINALS.add(SOUTH)
            ALL_CARDINALS.add(EAST)
            ALL_CARDINALS.add(WEST)
        }
    }
}
