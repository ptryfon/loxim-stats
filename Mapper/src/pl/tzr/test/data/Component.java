package pl.tzr.test.data;

import pl.tzr.transparent.structure.annotation.Reference;

public interface Component {

	int getAmount();

	void setAmount(int amount);
	
	@Reference
	Part getLeadsTo();
	
	void setLeadsTo(Part part);

}