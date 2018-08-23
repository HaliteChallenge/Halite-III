package hlt.command;

public class Construct implements Command {
    private int id;

    public Construct(int id) {
        this.id = id;
    }

    @Override
    public void build(StringBuilder sb) {
        sb.append("c ");
        sb.append(id);
    }
}
