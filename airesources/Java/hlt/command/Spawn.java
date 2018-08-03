package hlt.command;

public class Spawn implements Command {
    private int amount;

    public Spawn(int amount) {
        this.amount = amount;
    }

    @Override
    public void build(StringBuilder sb) {
        sb.append("g ");
        sb.append(amount);
    }
}
