package pl.tzr.test.data;

public class ComponentImpl implements Component {

	private int amount;
	
	private Part leadsTo;

	public int getAmount() {
		return amount;
	}

	public void setAmount(int amount) {
		this.amount = amount;
	}

	public Part getLeadsTo() {
		return leadsTo;
	}

	public void setLeadsTo(Part leadsTo) {
		this.leadsTo = leadsTo;
	}
	
}
