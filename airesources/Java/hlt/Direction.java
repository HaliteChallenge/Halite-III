package hlt;

import java.util.concurrent.ThreadLocalRandom;

public enum Direction {
    NORTH('n'),
    SOUTH('s'),
    EAST('e'),
    WEST('w'),
    STILL('o');
    private char charRepresent;
    private Direction(char c) {
        charRepresent = c;
    }
    public char getCharRepresent() {
        return charRepresent;
    }

    public static Direction randomDirection() {
        int randInt = ThreadLocalRandom.current().nextInt(0, 5);
        switch(randInt) {
        case 0: return Direction.NORTH;
        case 1: return Direction.SOUTH;
        case 2: return Direction.EAST;
        case 3: return Direction.WEST;
        case 4: return Direction.STILL;
        }
        return null;
    }
}
