package hlt.command;

public class Spawn implements Command {
    public Spawn() {
    }

    @Override
    public void build(StringBuilder sb) {
        sb.append("g");
    }
}
