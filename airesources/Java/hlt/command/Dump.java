package hlt.command;

public class Dump implements Command {
    private int id;
    private int amount;

    public Dump(int id, int amount) {
        this.id = id;
        this.amount = amount;
    }

    @Override
    public void build(StringBuilder sb) {
        sb.append("d ");
        sb.append(id);
        sb.append(" ");
        sb.append(amount);
    }
}
