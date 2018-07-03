package hlt;

public class Cell {
	private int production;
	private boolean passable;
	private int energyFactor;

	public Cell(int pr, boolean pa, int e) {
		production = pr;
		passable = pa;
		energyFactor = e;
	}

	public int getProduction() { return production; }
	public boolean getPassable() { return passable; }
	public int getEnergyFactor() { return energyFactor; }
}
