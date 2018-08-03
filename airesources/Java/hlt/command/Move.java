package hlt.command;

import hlt.Direction;

public class Move implements Command {
    private int id;
    private hlt.Direction direction;

    public Move(int id, hlt.Direction direction) {
        this.id = id;
        this.direction = direction;
    }

    @Override
    public void build(StringBuilder sb) {
        if (direction != Direction.STILL) {
            sb.append("m ");
            sb.append(id);
            sb.append(" ");
            sb.append(direction.getCharRepresent());
        }
    }
}
